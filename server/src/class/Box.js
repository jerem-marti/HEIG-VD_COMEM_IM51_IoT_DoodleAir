export default class Box {
    constructor(
        x1, 
        y1, 
        x2, 
        y2, 
        color = {
            r: 200,
            g: 100,
            b: 255,
            a: 0.1
        }) {
        this.x1 = x1;
        this.y1 = y1;
        this.x2 = x2;
        this.y2 = y2;
        this.color = color;
    }

    // draw the box
    draw(ctx) {
        ctx.fillStyle = `rgba(${this.color.r}, ${this.color.g}, ${this.color.b}, ${this.color.a})`;	
        ctx.fillRect(this.x1, this.y1, this.x2 - this.x1, this.y2 - this.y1);
    }

    // check if the point is inside the box
    isInside({x, y}) {
        console.log(this.x1, this.x2, this.y1, this.y2);
        console.log(Math.min(this.x1, this.x2) < x, x < Math.max(this.x1, this.x2), Math.min(this.y1, this.y2) < y, y < Math.max(this.y1, this.y2));
        if(Math.min(this.x1, this.x2) < x && x < Math.max(this.x1, this.x2) && Math.min(this.y1, this.y2) < y && y < Math.max(this.y1, this.y2)) {
            return true;
        }
    }

    // update the end corner of the box
    updateEndCorner(x2, y2) {
        this.x2 = x2;
        this.y2 = y2;
    }

    // increase the alpha of the box
    alphaUp() {
        this.color.a = 0.6;
    }

}