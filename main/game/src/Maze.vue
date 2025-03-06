<script setup>
import * as THREE from 'three';
import { onMounted, ref } from 'vue';
import { getRoll, getPitch } from './movement'; // 🔹 Import du module de mouvements

let scene, camera, renderer, maze, ball;

const ballRadius = 0.2; // Rayon de la bille
let ballVelocity = new THREE.Vector3(0, 0, 0); // Vitesse de la bille
const maxSpeed = 0.5; // 🔹 Vitesse maximale autorisée pour éviter une accélération incontrôlable
const gravity = 0.002; // Gravité de la bille sur le labyrinthe
const canvasRef = ref(null); // Permet d'attacher le canvas Vue

let walls = [];
let holes = [];
let shieldActive = false;

onMounted(() => {
    // Initialisation de Three.js
    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
    camera.position.set(0, 5, 5);
    camera.lookAt(0, 0, 0);

    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.setPixelRatio(window.devicePixelRatio); // Rend le canvas responsive
    canvasRef.value.appendChild(renderer.domElement);

    // Labyrinthe (plateforme)
    const mazeGeometry = new THREE.BoxGeometry(5, 0.2, 5);
    const mazeMaterial = new THREE.MeshStandardMaterial({ color: 0x00ff00 });
    maze = new THREE.Mesh(mazeGeometry, mazeMaterial);
    scene.add(maze);

    // 🔹 Ajouter les trous comme enfants du labyrinthe
    const holePositions = [
        { x: 1, z: 1 },
        { x: -1.5, z: -1 },
        { x: 2, z: -2 },
    ];

    holePositions.forEach(pos => {
        const holeGeometry = new THREE.CylinderGeometry(0.3, 0.3, 0.2001, 32);
        const holeMaterial = new THREE.MeshStandardMaterial({ color: 0x000000 });
        let hole = new THREE.Mesh(holeGeometry, holeMaterial);
        hole.position.set(pos.x, 0, pos.z); // Positionner sous la plateforme
        maze.add(hole); // 🔹 Ajouter le trou au labyrinthe (et non à la scène)
        holes.push(hole);
    });

    // 🔹 Ajouter les murs comme enfants du labyrinthe
    const wallPositions = [
        { x: -2.5, z: 0, width: 0.2, height: 5 }, // Mur gauche
        { x: 2.5, z: 0, width: 0.2, height: 5 },  // Mur droit
        { x: 0, z: -2.5, width: 5, height: 0.2 }, // Mur bas
        { x: 0, z: 2.5, width: 5, height: 0.2 },  // Mur haut
    ];

    wallPositions.forEach(pos => {
        const wallGeometry = new THREE.BoxGeometry(pos.width, 0.5, pos.height);
        const wallMaterial = new THREE.MeshStandardMaterial({ color: 0x555555 });
        let wall = new THREE.Mesh(wallGeometry, wallMaterial);
        wall.position.set(pos.x, 0.3, pos.z);
        maze.add(wall); // 🔹 Ajouter le mur au labyrinthe (et non à la scène)
        walls.push(wall);
    });

    // Bille
    const ballGeometry = new THREE.SphereGeometry(ballRadius, 32, 32);
    const ballMaterial = new THREE.MeshStandardMaterial({ color: 0xff0000 });
    ball = new THREE.Mesh(ballGeometry, ballMaterial);
    ball.position.set(0, 0.3, 0); // Toujours au bon niveau du labyrinthe
    maze.add(ball); // 🔹 Ajout de la bille au labyrinthe pour qu'elle suive sa rotation

    // Lumière
    const light = new THREE.DirectionalLight(0xffffff, 1);
    light.position.set(5, 5, 5);
    scene.add(light);

    animate();
});

// Gestion des boutons Interior (Boost) et Exterior (Bouclier)
window.addEventListener("button-action", (event) => {
    if (event.detail.button === "interior" && event.detail.state === "pressed") {
        console.log("🏎️ Boost activé !");
        ballVelocity.multiplyScalar(2); // Double la vitesse
        setTimeout(() => ballVelocity.multiplyScalar(0.5), 2000); // Ralentit après 2s
    }

    if (event.detail.button === "exterior" && event.detail.state === "pressed") {
        console.log("🛡️ Bouclier activé !");
        shieldActive = true;
        setTimeout(() => {
            shieldActive = false;
            console.log("⏳ Bouclier expiré !");
        }, 5000); // Dure 5 secondes
    }
});

// Fonction d'animation
function animate() {
    requestAnimationFrame(animate);

    // 🔹 Rotation du labyrinthe en fonction des valeurs IMU
    maze.rotation.x = THREE.MathUtils.degToRad(getPitch());
    maze.rotation.z = THREE.MathUtils.degToRad(getRoll());

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
            ball.position.x - ballRadius < wallMaxX && ball.position.x + ballRadius > wallMinX &&
            ball.position.z - ballRadius < wallMaxZ && ball.position.z + ballRadius > wallMinZ
        ) {
            console.log("🚧 Collision avec un mur !");

            // 🔹 Correction de la position en fonction de la direction du mouvement
            const previousX = ball.position.x - ballVelocity.x;
            const previousZ = ball.position.z - ballVelocity.z;

            if (previousX < wallMinX) {
                ball.position.x = wallMinX - ballRadius;
            } else if (previousX > wallMaxX) {
                ball.position.x = wallMaxX + ballRadius;
            }

            if (previousZ < wallMinZ) {
                ball.position.z = wallMinZ - ballRadius;
            } else if (previousZ > wallMaxZ) {
                ball.position.z = wallMaxZ + ballRadius;
            }

            // 🔹 Appliquer un rebond réaliste
            ballVelocity.x *= -0.4;
            ballVelocity.z *= -0.4;
        }
    });

    // 🔹 Vérification si la bille tombe dans un trou
    holes.forEach(hole => {
        if (hole && hole.position && ball.position.distanceTo(new THREE.Vector3(hole.position.x, ball.position.y, hole.position.z)) < ballRadius) {
            if (shieldActive) {
                console.log("🔥 Bouclier a empêché la chute !");
                shieldActive = false;
                ball.position.set(ball.position.x, 0.3, ball.position.z); // Stoppe la chute
            } else {
                console.log("🕳️ Bille en train de tomber dans un trou !");
                ballVelocity.set(0, 0, 0); // Supprimer la vitesse pour éviter une fuite latérale
                ball.position.y -= 0.02; // 🔹 Fait descendre la bille progressivement
            }
        }
    });

    // 🔹 Réinitialisation de la bille si elle tombe totalement sous le plateau
    if (ball.position.y < -1) {
        console.log("❌ Bille tombée dans un trou, réinitialisation !");
        ball.position.set(0, 0.3, 0); // Réinitialisation
        ballVelocity.set(0, 0, 0);
    }

    renderer.render(scene, camera);
}

// Permet de redimensionner le canvas en cas de changement de taille
window.addEventListener('resize', () => {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
});

</script>

<template>
    <div ref="canvasRef"></div>
</template>

<style>
div { width: 100vw; height: 100vh; }
</style>
