import { TAU } from "/utils/math.js";

export default class Circle {
  constructor({x, y, r, speed = 0, dir = 0, color}) {
    this.x = x;
    this.y = y;
    this.r = r;
    this.color = color;
    this.speed = speed;
    this.dir = dir; // radian
  }

  // get the radius of the circle
  getRadius() {
    return this.r;
  }

  // get the speed of the circle
  setSpeed(speed){
    this.speed = speed;
  }

  // set the color of the circle
  setColor(color){
    this.color = color;
  }

  // set the direction of the circle
  setDir(dir){
    this.dir = dir;
  }

  // compare the radius of the circle
  compareTo(otherCircle) {
    // test instanceof ?
    return this.getRadius() - otherCircle.getRadius();
  }

  // get the distance between the circle and a point
  distanceTo({x , y}) {
    const dx = this.x - x;
    const dy = this.y - y;
    return Math.sqrt(dx*dx + dy*dy);
  }

  // check if the point is inside the circle
  isInside({x, y}) {
    return this.distanceTo({x, y}) < this.r;
  }

  // draw the circle
  draw(ctx) {
    ctx.beginPath();
    ctx.fillStyle = `rgba(${this.color.r}, ${this.color.g}, ${this.color.b}, ${this.color.a})`;
    ctx.arc(this.x, this.y, this.r, 0, TAU);
    ctx.closePath();
    ctx.fill();
  }

  // move the circle
  move(deltaT) {
    const distX = this.speed * deltaT * Math.cos(this.dir);
    const distY = this.speed * deltaT * Math.sin(this.dir);
    this.x += distX;
    this.y += distY;
  }

}