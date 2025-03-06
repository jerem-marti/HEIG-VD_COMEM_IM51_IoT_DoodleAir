const ws = new WebSocket("ws://localhost:8080"); // Connexion WebSocket

ws.onopen = () => {
    console.log("✅ WebSocket connecté depuis Vue.js !");
};

ws.onerror = (error) => console.error("❌ Erreur WebSocket :", error);
ws.onclose = () => console.warn("⚠️ WebSocket déconnecté !");

ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    console.log("📨 Données reçues dans Vue.js :", data);

    if ("roll" in data && "pitch" in data) {
        // Événement pour la mise à jour du tilt avec inversion (voir étape 2)
        window.dispatchEvent(new CustomEvent("update-tilt", { detail: data }));
    }
    if ("potentiometer" in data) {
        window.dispatchEvent(new CustomEvent("update-speed", { detail: data.potentiometer }));
    }
    if ("button" in data) {
        window.dispatchEvent(new CustomEvent("button-action", { detail: data }));
    }
};
