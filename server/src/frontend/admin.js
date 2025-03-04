import MainLoop from '../utils/mainloop.js';
import Keyboard from '../class/Keyboard.js';
import Mouse from '../class/Mouse.js';
//import Box from '../class/Box.js';
//import Circle from '../class/Circle.js';
import WSClient from '../websocket/WSClient.js';

//Create a canvas and set its size to the size of the window
const ctx = document.querySelector('canvas').getContext('2d');
ctx.canvas.width = ctx.canvas.clientWidth;
ctx.canvas.height = ctx.canvas.clientHeight;

//Create a timestep for the physics engine
const timestep = 1000/120;

//Create a keyboard object to manage user input
const keyboard = new Keyboard();

// Create a mouse object to manage mouse input
const mouse = new Mouse(ctx.canvas);

//Stores boxes
const boxes = new Map();

//Staging box during the hold of the mouse
let stagingBox = null;

//Stores circles
const circles = new Map();

//Create a websocket client
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
wsClient.sub("doodle", (data) => {
    //test if in box
    console.log("Received data:", data);
});

MainLoop.setUpdate((dt) => {
   
});

MainLoop.setDraw(() => {
    
});

MainLoop.start();




