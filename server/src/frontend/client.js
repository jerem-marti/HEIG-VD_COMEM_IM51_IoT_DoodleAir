import WSClient from "../websocket/WSClient.js";

const wsClient = new WSClient(import.meta.env.VITE_WS_URL);
await wsClient.connect();
wsClient.on("close", () => {
  console.log("Connection closed");
});
wsClient.on("open", () => {
  console.log("Connection opened");
});
wsClient.on("error", (err) => {
  console.error(err);
});

//Create a canvas and set its size to the size of the window
const ctx = document.querySelector("canvas").getContext("2d");
ctx.canvas.width = ctx.canvas.clientWidth;
ctx.canvas.height = ctx.canvas.clientHeight;

//listen on mouse click and send a message to the server
ctx.canvas.addEventListener("click", (e) => {
  //wsClient.pub("heatmap", { x: e.clientX, y: e.clientY });
  wsClient.pub("doodle", { x: e.clientX / ctx.canvas.width, y: e.clientY / ctx.canvas.height });
});
