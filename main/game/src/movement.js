// 🔹 Stocker les valeurs précédentes pour un lissage
let roll = 0;
let pitch = 0;
let alpha = 0.5; // Facteur de lissage (plus bas = plus lisse, plus haut = plus réactif)
let sensitivity = 0.5; // Facteur de sensibilité par défaut (entre 0.05 et 1.5)

// 🔹 Mapping de la sensibilité (potentiomètre : 0 → 4000+  =>  Sensibilité : 0.05 → 1.5)
function mapSensitivity(potValue) {
    const minPot = 0;
    const maxPot = 4000;
    const minSensitivity = 0.05;
    const maxSensitivity = 1.5;

    // 🔹 Normalisation entre 0 et 1
    let normalized = Math.min(Math.max((potValue - minPot) / (maxPot - minPot), 0), 1);
    return minSensitivity + normalized * (maxSensitivity - minSensitivity);
}

// 🔹 Écoute des événements WebSocket pour le potentiomètre
window.addEventListener("update-speed", (event) => {
    sensitivity = mapSensitivity(event.detail); // Met à jour la sensibilité
    console.log(`🎚️ Sensibilité ajustée : ${sensitivity.toFixed(2)}`);
});

// 🔹 Écoute des événements WebSocket pour l'inclinaison
window.addEventListener("update-tilt", (event) => {
    const rawRoll = event.detail.pitch; // Inversion
    const rawPitch = event.detail.roll; // Inversion

    // 🔹 Appliquer un lissage exponentiel + sensibilité
    roll = alpha * (rawRoll * sensitivity) + (1 - alpha) * roll;
    pitch = alpha * (rawPitch * sensitivity) + (1 - alpha) * pitch;

    console.log("🎮 Valeurs lissées et ajustées :", { roll, pitch });

    // 🔹 Envoyer les nouvelles valeurs lissées et ajustées
    window.dispatchEvent(new CustomEvent("smooth-tilt", { detail: { roll, pitch } }));
});

// 🔹 Export des valeurs filtrées
export function getRoll() {
    return roll;
}

export function getPitch() {
    return pitch;
}

export function getSensitivity() {
    return sensitivity;
}