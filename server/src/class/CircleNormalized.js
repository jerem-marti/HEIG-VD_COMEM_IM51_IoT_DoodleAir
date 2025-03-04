import Circle from './Circle';

// Normalize the circle to the canvas size
export default class CircleNormalized extends Circle { 
  // draw the circle with normalized position
  draw(ctx) {
    ctx.beginPath();
    ctx.fillStyle = `rgba(${this.color.r}, ${this.color.g}, ${this.color.b}, ${this.color.a})`;
    ctx.arc(this.x * ctx.canvas.width, this.y * ctx.canvas.height, this.r, 0, 2 * Math.PI);
    ctx.closePath();
    ctx.fill();
  }
}