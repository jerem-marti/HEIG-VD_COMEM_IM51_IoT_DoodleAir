import WSServerPubSub from '../websocket/WSServerPubSub.mjs';

function hookPub(msg, client, wsServer) {
  if (typeof msg !== 'string' || msg.length < 1) return false;
  const timestamp = new Date().getTime();
  console.log(`📩 Message reçu: ${msg} de ${client?.username || "inconnu"}`);
  return { msg, timestamp, username: client?.username || "inconnu" };
}

const wsServer = new WSServerPubSub({
  port: 8080,
  origins: '*',
  path: '/',  // Assurez-vous que le chemin correspond à l'ESP32
});

wsServer.addChannel('doodle', {
  usersCanPub: true,
  usersCanSub: true,
  hookPub
});

// Log pour s'assurer que le serveur est bien lancé
wsServer.on('start', () => console.log("🚀 Serveur WebSocket Doodle démarré sur ws://localhost:8080"));

// Gestion des connexions
wsServer.on('connection', (client) => {
  console.log(`✅ Client connecté : ${client.id}`);
});

// Gestion des déconnexions
wsServer.on('close', (client) => {
  console.log(`🔴 Client déconnecté : ${client.id}`);
});

wsServer.start();