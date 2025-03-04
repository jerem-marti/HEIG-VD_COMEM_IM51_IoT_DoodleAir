import WSServerPubSub from '../websocket/WSServerPubSub.mjs';

function hookPub(msg, client, wsServer) {
  if (!msg) return false;
  const timestamp = new Date().getTime();
  console.log('msg:', msg);
  return {
    ...msg,
    serverTimestamp: timestamp,
    username: client.username || 'esp32-client'
  };
}

const wsServer = new WSServerPubSub({
  port: 8887,
  origins: '*',  // Accept connections from any origin
  path: '/ws'
});

wsServer.addChannel('heatmap', {
  usersCanPub: true,
  usersCanSub: true,
  hookPub: hookPub
});

wsServer.start();
