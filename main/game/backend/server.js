import express from 'express';
import { WebSocketServer } from 'ws';

const app = express();
const PORT = 3000;

// Serveur WebSocket
const wss = new WebSocketServer({ port: 8080 });

wss.on('connection', (ws) => {
    console.log("✅ Un client WebSocket vient de se connecter !");
    
    ws.on('message', (data) => {
        const msg = JSON.parse(data);
        
        // Si l'ESP32 envoie un message d'identification
        if (msg.device === "ESP32") {
            console.log("🛰️ ESP32 détecté !");
            ws.isESP32 = true; // Ajoute une propriété à la connexion
        }

        console.log("📨 Données reçues :", msg);

        // Broadcast des messages à tous les clients sauf l'ESP32 (pour éviter les boucles)
        wss.clients.forEach(client => {
            if (client !== ws && client.readyState === ws.OPEN) {
                client.send(JSON.stringify(msg));
            }
        });
    });

    ws.on('close', () => {
        console.log(ws.isESP32 ? "❌ ESP32 déconnecté !" : "❌ Un client Vue.js déconnecté !");
    });
});

// Serveur Express (optionnel pour API ou statiques)
app.listen(PORT, () => console.log(`🚀 Serveur sur http://localhost:${PORT}`));
