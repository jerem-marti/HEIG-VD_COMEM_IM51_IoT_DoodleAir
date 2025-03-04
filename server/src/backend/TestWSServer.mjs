import { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 8080 });

wss.on('connection', (ws) => {
  console.log("✅ ESP32 connecté !");
  
  ws.on('message', (message) => {
    console.log(`📩 Message reçu: ${message}`);
    ws.send(`👋 Echo: ${message}`);
  });

  ws.on('close', () => {
    console.log("🔴 ESP32 déconnecté");
  });
});

console.log("🚀 Serveur WebSocket démarré sur ws://192.168.15.13:8080");
