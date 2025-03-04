import Box from "./Box";

// BoxNormalized class
export default class BoxNormalized extends Box {
    // refactor the draw method to draw the box with normalized position
    draw(ctx) {
        ctx.fillStyle = `rgba(${this.color.r}, ${this.color.g}, ${this.color.b}, ${this.color.a})`;	
        ctx.fillRect(this.x1 * ctx.canvas.width, this.y1 * ctx.canvas.height, (this.x2 - this.x1) * ctx.canvas.width, (this.y2 - this.y1) * ctx.canvas.height);
    }
}