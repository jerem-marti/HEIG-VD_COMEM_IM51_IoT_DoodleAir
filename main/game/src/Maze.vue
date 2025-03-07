<script setup>
import * as THREE from 'three';
import { onMounted, ref } from 'vue';
import { getRoll, getPitch, getSensitivity } from './movement'; // 🔹 Import du module de mouvements

let scene, camera, renderer, maze, ball;

const ballRadius = 40; // Rayon de la bille
const holeRadius = 60; // Rayon des trous
let ballVelocity = new THREE.Vector3(0, 0, 0); // Vitesse de la bille
const maxSpeed = 0.5; // 🔹 Vitesse maximale autorisée pour éviter une accélération incontrôlable
const gravity = 0.002; // Gravité de la bille sur le labyrinthe
const canvasRef = ref(null); // Permet d'attacher le canvas Vue

const MAZE_SIZE = { width: 2000, height: 2000 }; // Taille du labyrinthe
const SCALE = 0.05; // Réduction pour Three.js (évite d'être trop grand)

let start = { x: 0, y: 0 };
let end = { x: 0, y: 0 };
let walls = [];
let holes = [];

// 📌 Variables du jeu
const gameState = ref("waiting"); // waiting | playing | won | lost
const bestScoreData = ref({ username: "", time: null });
const lastTime = ref(0);
const startTime = ref(null);
let gameInterval = null; // 🔹 Référence pour timer
const username = ref("");
const bonuses = ref({ shield: 1, boost: 2 });
const shieldTimeLeft = ref(0); // 🔹 Temps restant du bouclier
let shieldInterval = null; // 🔹 Référence pour le bouclier
const boostTimeLeft = ref(0);  // 🔹 Temps restant du boost
let boostInterval = null; // 🔹 Référence pour le boost
let shieldActive = false;

// 📌 Fonction pour charger le JSON et générer le labyrinthe
async function loadMaze() {
    const response = await fetch("/maze_data.json"); // 🔹 Charge le fichier JSON
    const mazeData = await response.json();
    start = mazeData.start;
    end = mazeData.end;

    // 📌 Création de la structure du labyrinthe
    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
    camera.position.set(0, 150, 0);
    camera.lookAt(0, 0, 0);

    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.setPixelRatio(window.devicePixelRatio); // Rend le canvas responsive
    canvasRef.value.appendChild(renderer.domElement);

    // Charger la texture du bois pour le labyrinthe
    const textureLoader = new THREE.TextureLoader();
    const woodTexture = textureLoader.load(
        "/textures/oak_veneer_01.jpg",
        () => console.log("✅ Texture chargée avec succès !"),
        undefined,
        (error) => console.error("❌ Erreur de chargement de la texture :", error)
    );
    woodTexture.wrapS = THREE.RepeatWrapping;
    woodTexture.wrapT = THREE.RepeatWrapping;
    woodTexture.repeat.set(2, 2);

    // Labyrinthe (plateforme)
    const mazeGeometry = new THREE.BoxGeometry(MAZE_SIZE.width * SCALE, 4, MAZE_SIZE.height * SCALE);
    const mazeMaterial = new THREE.MeshStandardMaterial({ 
        map: woodTexture,
        roughness: 0.6,  // Ajuste la rugosité pour un effet plus réaliste
        metalness: 0.1   // Donne un léger effet de brillance
    });
    maze = new THREE.Mesh(mazeGeometry, mazeMaterial);
    scene.add(maze);

    // 📌 Ajout des trous
    mazeData.holes.forEach(pos => {
        const holeGeometry = new THREE.CylinderGeometry(holeRadius * SCALE, holeRadius * SCALE, 4.1, 32);
        const holeMaterial = new THREE.MeshStandardMaterial({ color: 0x000000 });
        let hole = new THREE.Mesh(holeGeometry, holeMaterial);
        hole.position.set((pos.x - MAZE_SIZE.width / 2) * SCALE, 0, (pos.y - MAZE_SIZE.height / 2) * SCALE);
        maze.add(hole);
        holes.push(hole);
    });

    // 📌 Ajout des murs
    mazeData.walls.forEach(pos => {
        if(Math.abs(pos.x2 - pos.x1) < 0.1) {
            pos.x1 -= 10;
            pos.x2 += 10;
        } else if(Math.abs(pos.y2 - pos.y1) < 0.1) {
            pos.y1 -= 10;
            pos.y2 += 10;
        }
        const wallGeometry = new THREE.BoxGeometry(
            Math.abs(pos.x2 - pos.x1) * SCALE, 
            8, 
            Math.abs(pos.y2 - pos.y1) * SCALE
        );
        const wallMaterial = new THREE.MeshStandardMaterial({ 
            map: woodTexture,
            roughness: 0.6,
            metalness: 0.1
        });
        let wall = new THREE.Mesh(wallGeometry, wallMaterial);
        wall.position.set(
            ((pos.x1 + pos.x2) / 2 - MAZE_SIZE.width / 2) * SCALE, 
            2, 
            ((pos.y1 + pos.y2) / 2 - MAZE_SIZE.height / 2) * SCALE
        );
        maze.add(wall);
        walls.push(wall);
    });

    // 📌 Ajout de la bille
    const ballGeometry = new THREE.SphereGeometry(ballRadius * SCALE, 32, 32);
    const ballMaterial = new THREE.MeshStandardMaterial({ color: 0xff0000 });
    ball = new THREE.Mesh(ballGeometry, ballMaterial);
    ball.position.set((start.x - MAZE_SIZE.width / 2) * SCALE, 4, (start.y - MAZE_SIZE.height / 2) * SCALE);
    maze.add(ball);

    // 🔹 Lumière ambiante : éclaire toute la scène pour éviter les zones noires
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
    scene.add(ambientLight);

    // 🔹 Lumière directionnelle principale (simule une lumière du soleil)
    const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
    directionalLight.position.set(100, 200, 100);
    directionalLight.castShadow = true;  // Active les ombres
    directionalLight.shadow.mapSize.width = 2048;
    directionalLight.shadow.mapSize.height = 2048;
    directionalLight.shadow.camera.near = 0.5;
    directionalLight.shadow.camera.far = 500;
    scene.add(directionalLight);

    // 🔹 Lumière ponctuelle au-dessus du plateau (crée un effet de halo)
    const pointLight = new THREE.PointLight(0xffffff, 0.8, 500);
    pointLight.position.set(0, 150, 0);
    scene.add(pointLight);

    // 🔹 Lumière supplémentaire pour éclairer les murs sous un autre angle
    const sideLight = new THREE.DirectionalLight(0xffffff, 0.5);
    sideLight.position.set(-100, 100, -100);
    scene.add(sideLight);

    // 🔹 Activer le rendu des ombres sur les objets
    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap; // Ombrage plus doux

    // Activer les ombres sur les objets
    maze.castShadow = true;
    maze.receiveShadow = true;
    walls.forEach(wall => {
        wall.castShadow = true;
        wall.receiveShadow = true;
    });
    ball.castShadow = true;
    ball.receiveShadow = true;


    animate();
}

onMounted(() => {
    loadBestScore(); // 🔹 Charge le meilleur score
    loadMaze(); // 🔹 Charge le labyrinthe au démarrage
});

// Fonction d'animation
function animate() {
    requestAnimationFrame(animate);

    // 🔹 Rotation du labyrinthe en fonction des valeurs IMU
    maze.rotation.x = THREE.MathUtils.degToRad(getPitch());
    maze.rotation.z = THREE.MathUtils.degToRad(getRoll());

    if(gameState.value === "playing") {
        // 🔹 Simulation de la gravité
        let gravityEffect = new THREE.Vector3(-getRoll() * gravity, 0, getPitch() * gravity);
        ballVelocity.add(gravityEffect);
        ballVelocity.clampLength(0, maxSpeed); // 🔹 Limite la vitesse de la bille
        ball.position.add(ballVelocity);

        // 🔹 Ajout de frottement (ralentit la bille progressivement)
        ballVelocity.multiplyScalar(0.98);

        // 🔹 Vérification des collisions avec les murs (prise en compte du rayon)
        walls.forEach(wall => {
            const wallMinX = wall.position.x - (wall.geometry.parameters.width / 2);
            const wallMaxX = wall.position.x + (wall.geometry.parameters.width / 2);
            const wallMinZ = wall.position.z - (wall.geometry.parameters.depth / 2);
            const wallMaxZ = wall.position.z + (wall.geometry.parameters.depth / 2);

            if (
                ball.position.x - ballRadius * SCALE < wallMaxX && ball.position.x + ballRadius * SCALE > wallMinX &&
                ball.position.z - ballRadius * SCALE < wallMaxZ && ball.position.z + ballRadius * SCALE > wallMinZ
            ) {
                console.log("🚧 Collision avec un mur !");

                // 🔹 Correction de la position en fonction de la direction du mouvement
                const previousX = ball.position.x - ballVelocity.x;
                const previousZ = ball.position.z - ballVelocity.z;

                let collisionOnX = false;
                let collisionOnZ = false;

                if (previousX < wallMinX || previousX > wallMaxX) {
                    ball.position.x = previousX < wallMinX ? wallMinX - ballRadius * SCALE : wallMaxX + ballRadius * SCALE;
                    ballVelocity.x *= -0.4; // 🔹 Appliquer un rebond uniquement sur X
                    collisionOnX = true;
                }

                if (previousZ < wallMinZ || previousZ > wallMaxZ) {
                    ball.position.z = previousZ < wallMinZ ? wallMinZ - ballRadius * SCALE : wallMaxZ + ballRadius * SCALE;
                    ballVelocity.z *= -0.4; // 🔹 Appliquer un rebond uniquement sur Z
                    collisionOnZ = true;
                }

                if (collisionOnX && collisionOnZ) {
                    console.log("💥 Coin touché !");
                }
            }
        });

        // 🔹 Vérification si la balle atteint l'arrivée (coordonnées de fin)
        const arrivalPosition = new THREE.Vector3((end.x - MAZE_SIZE.width / 2) * SCALE, ball.position.y, (end.y - MAZE_SIZE.height / 2) * SCALE);
        if (ball.position.distanceTo(arrivalPosition) < ballRadius * SCALE && gameState.value === "playing") {
            gameState.value = "won";
            clearInterval(gameInterval); // 🔴 Arrête la mise à jour du temps
            lastTime.value = (Date.now() - startTime.value) / 1000;
            
            updateBestScore(); // 🔹 Met à jour le meilleur score si nécessaire

            console.log(`🏆 Victoire en ${lastTime.value} secondes !`);
            
            // ⏳ Attendre 3 secondes avant de revenir à l'écran d'attente
            setTimeout(() => {
                gameState.value = "waiting";
            }, 3000);
        }

        let isInHole = false;
        // 🔹 Vérification si la bille tombe dans un trou
        holes.forEach(hole => {
            if (hole && hole.position && ball.position.distanceTo(new THREE.Vector3(hole.position.x, ball.position.y, hole.position.z)) < holeRadius * SCALE) {
                if (shieldActive) {
                    console.log("🔥 Bouclier a empêché la chute !");
                    ball.position.set(ball.position.x, 0.3, ball.position.z); // Stoppe la chute
                } else {
                    console.log("🕳️ Bille en train de tomber dans un trou !");
                    isInHole = true;

                    const deltaX = hole.position.x - ball.position.x;
                    const deltaZ = hole.position.z - ball.position.z;
                    const distanceToHole = Math.sqrt(deltaX * deltaX + deltaZ * deltaZ);
                    // calcule de la distance du centre de la bille au bord du trou
                    const distanceToEdge = holeRadius * SCALE - distanceToHole;
                    // calcule de la force de gravité horizontale en fonction du centre de masse de la bille
                    const horizontaleGravityForce = new THREE.Vector3(deltaX, 0, deltaZ).normalize().multiplyScalar(gravity);
                    // calcule de la force de frottement en fonction de la distance au bord du trou
                    const frictionForce = ballVelocity.clone().normalize().multiplyScalar(distanceToEdge * 0.01);
                    // calcule de la force gravité verticale en fonction de la distance au bord du trou
                    const verticalGravityForce = new THREE.Vector3(0, -gravity*150, 0).multiplyScalar(distanceToEdge);
                    // calcule de la force totale
                    const dropForce = horizontaleGravityForce.add(frictionForce).add(verticalGravityForce);
                    if (ball.position.y < 0.7 * ballRadius * SCALE) {
                        ballVelocity.multiplyScalar(0.5);
                    }
                    ballVelocity.add(dropForce); // Ajoute la force totale à la vitesse de la bille
                }
            }
        });
        if (!isInHole) {
            ball.position.y = 4; // Garde la bille au bon niveau
        }

        // 🔹 Réinitialisation de la bille si elle tombe totalement sous le plateau
        if (ball.position.y < 2 * -ballRadius * SCALE) { 
            console.log("❌ Bille tombée dans un trou, réinitialisation !");
            gameState.value = "lost";
            clearInterval(gameInterval); // 🔴 Arrête la mise à jour du temps
            ball.position.set((start.x - MAZE_SIZE.width / 2) * SCALE, 4, (start.y - MAZE_SIZE.height / 2) * SCALE);
            ballVelocity.set(0, 0, 0);
            
            // ⏳ Attendre 3 secondes avant de revenir à l'écran d'attente
            setTimeout(() => {
                gameState.value = "waiting";
            }, 3000);
        }
    }
    renderer.render(scene, camera);
}

// Permet de redimensionner le canvas en cas de changement de taille
window.addEventListener('resize', () => {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
});

// 🔹 Activer le bouclier pendant 8 secondes
function activateShield() {
    if (bonuses.value.shield > 0 && shieldTimeLeft.value === 0) {
        bonuses.value.shield--;
        shieldActive = true;
        shieldTimeLeft.value = 3; // Durée du bouclier

        shieldInterval = setInterval(() => {
            shieldTimeLeft.value--;
            if (shieldTimeLeft.value <= 0) {
                shieldActive = false;
                clearInterval(shieldInterval);
            }
        }, 1000);
    }
}

// 🔹 Activer le boost de vitesse pendant 3 secondes
function activateBoost() {
    if (bonuses.value.boost > 0 && boostTimeLeft.value === 0) {
        bonuses.value.boost--;
        boostTimeLeft.value = 3; // Durée du boost
        // Fait dash la bille en ajoutant à sa position la vitesse actuelle + le diamètre de la bille
        ball.position.add(ballVelocity.clone().normalize().multiplyScalar(ballRadius * 2 * SCALE));
        // test si ball position est hors du labyrinthe
        if (ball.position.x < -MAZE_SIZE.width / 2 * SCALE || ball.position.x > MAZE_SIZE.width / 2 * SCALE || ball.position.z < -MAZE_SIZE.height / 2 * SCALE || ball.position.z > MAZE_SIZE.height / 2 * SCALE) {
            ball.position.sub(ballVelocity.clone().normalize().multiplyScalar(ballRadius * 2 * SCALE));
        }
        boostInterval = setInterval(() => {
            boostTimeLeft.value--;
            if (boostTimeLeft.value <= 0) {
                ballVelocity.multiplyScalar(0.5); // Remet la vitesse normale
                clearInterval(boostInterval);
            }
        }, 1000);
    }
}

// 🔹 Gestion des boutons pour activer les bonus
window.addEventListener("button-action", (event) => {
    if (event.detail.button === "interior" && event.detail.state === "pressed") {
        activateShield();
    }

    if (event.detail.button === "exterior" && event.detail.state === "pressed") {
        activateBoost();
    }
});

function startGame() {
    if (!username.value.trim()) {
        alert("⚠️ Merci d'entrer un pseudo !");
        return;
    }

    console.log(`🚀 Nouvelle partie pour ${username.value} !`);
    gameState.value = "playing";
    ball.position.set((start.x - MAZE_SIZE.width / 2) * SCALE, 4, (start.y - MAZE_SIZE.height / 2) * SCALE);
    ballVelocity.set(0, 0, 0);
    startTime.value = Date.now();
    lastTime.value = 0;

    clearInterval(boostInterval); // 🔹 Réinitialise le boost
    clearInterval(shieldInterval); // 🔹 Réinitialise le bouclier
    bonuses.value = { shield: 1, boost: 2 }; // 🔹 Réinitialise les bonus
    console.log(bonuses.value.shield);
    console.log(bonuses.value.boost);

    // 🔹 Met à jour le temps toutes les 100ms
    if (gameInterval) clearInterval(gameInterval);
    gameInterval = setInterval(() => {
        if (gameState.value === "playing") {
            lastTime.value = (Date.now() - startTime.value) / 1000;
        }
    }, 100);
}

// 🔹 Charger le meilleur score au démarrage
function loadBestScore() {
    const storedData = localStorage.getItem("bestScore");
    if (storedData) {
        bestScoreData.value = JSON.parse(storedData);
    }
}

// 🔹 Mettre à jour le meilleur score si nécessaire
function updateBestScore() {
    if (!bestScoreData.value.time || lastTime.value < bestScoreData.value.time) {
        bestScoreData.value = { username: username.value, time: lastTime.value };
        localStorage.setItem("bestScore", JSON.stringify(bestScoreData.value));
    }
}
</script>

<template>
    <div class="game-container">
        <div ref="canvasRef"></div>

        <!-- 🏆 Upper Sheet (Meilleur score + timer) -->
        <div class="upper-sheet">
            <div class="best-score">
                🏆 Meilleur score : 
                <span v-if="bestScoreData.time">{{ bestScoreData.username }} - {{ bestScoreData.time.toFixed(2) }}s</span>
                <span v-else>Aucun</span>
            </div>
        <div class="current-time">{{ lastTime.toFixed(2) + "s" }}</div>
        </div>

        <!-- 🎮 Bottom Sheet (Pseudo + Jouer ou Bonus en jeu) -->
        <div class="bottom-sheet">
            <div v-if="gameState === 'waiting'">
                <input type="text" v-model="username" placeholder="Entre ton pseudo..." class="username-input">
                <button @click="startGame" class="play-button">▶️ Jouer</button>
            </div>
            <div v-if="gameState === 'playing'" class="bonus-container">
                <!-- 🛡 Bouclier -->
                <div class="bonus shield" :class="bonuses.shield > 0 ? 'active' : 'inactive'">
                    {{ bonuses.shield }} 🛡
                    <div class="progress-bar">
                        <div class="progress" :style="{ width: (shieldTimeLeft / 8) * 100 + '%' }"></div>
                    </div>
                </div>

                <!-- 🚀 Boost -->
                <div class="bonus boost" :class="bonuses.boost > 0 ? 'active' : 'inactive'">
                    {{ bonuses.boost }} 🚀
                    <div class="progress-bar">
                        <div class="progress" :style="{ width: (boostTimeLeft / 3) * 100 + '%' }"></div>
                    </div>
                </div>
            </div>
        </div>

        <!-- Message de victoire -->
        <div class="dialog victory" v-if="gameState === 'won'">
            <h1>🎉 Bravo !</h1>
            <p>Temps : {{ lastTime.toFixed(2) }}s</p>
        </div>

        <!-- Message de défaite -->
        <div class="dialog defeat" v-if="gameState === 'lost'">
            <h1>😢 Perdu !</h1>
            <p>Essaie encore !</p>
        </div>
    </div>
</template>

<style>
.game-container {
    position: relative;
    width: 100vw;
    height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    overflow: hidden;
}

/* 🔹 Upper Sheet (Top) */
.upper-sheet {
    position: absolute;
    top: 0;
    width: 100dvw;
    background: #fdf5e68b; /* Blanc cassé */
    padding: 1rem 0;
    text-align: center;
    font-size: 1.2rem;
    font-weight: bold;
    box-shadow: 0px 2px 10px rgba(0, 0, 0, 0.2);
    border-radius: 0 0 2rem 2rem;
}

.best-score {
    background: gold;
    padding: 0.5rem;
    margin: 0.5rem;
    border-radius: 0.5rem;
    color: black;
}

/* 🔹 Bottom Sheet (Bottom) */
.bottom-sheet {
    position: absolute;
    bottom: 0;
    width: 100dvw;
    background: #fdf5e68b; /* Blanc cassé */
    padding: 1rem 0;
    text-align: center;
    font-size: 1.2rem;
    box-shadow: 0px -2px 10px rgba(0, 0, 0, 0.2);
    border-radius: 2rem 2rem 0 0;
}

.username-input {
    width: 80%;
    padding: 0.5rem;
    margin: 0.5rem;
    margin-right: 0;
    border: 2px solid #ccc;
    border-radius: 0.5rem;
    font-size: 1.2rem;
}

.play-button {
    background: #4CAF50;
    color: white;
    font-size: 1.2rem;
    padding: 0.5rem 1rem;
    margin: 0.5rem;
    border: none;
    border-radius: 0.5rem;
    cursor: pointer;
}

.bonus-container {
    display: flex;
    justify-content: center;
    gap: 1rem;
}

.bonus {
    width: 4rem;
    height: 2rem;
    text-align: center;
    line-height: 2rem;
    font-weight: bold;
    border-radius: 0.5rem;
    position: relative;
    overflow: hidden;
}

.bonus.active {
    background: #2196F3; /* Bleu pour shield */
    color: white;
}

.bonus.inactive {
    background: #ccc;
    color: #888;
}

.bonus.boost.active {
    background: #F44336; /* Rouge pour boost */
}

/* Barre de progression */
.progress-bar {
    position: absolute;
    bottom: 0;
    left: 0;
    width: 100%;
    height: 5px;
    background: rgba(255, 255, 255, 0.3);
}

.progress {
    height: 100%;
    background: white;
    transition: width 1s linear;
}

.dialog {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    background: white;
    color: black;
    padding: 2rem;
    border-radius: 2rem;
    box-shadow: 0 5px 15px rgba(0, 0, 0, 0.3);
    text-align: center;
}

.dialog.victory { border: 2px solid gold; }
.dialog.defeat { border: 2px solid red; }
</style>
