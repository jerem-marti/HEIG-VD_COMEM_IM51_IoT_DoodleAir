export default class Mouse {
    constructor(canvas) {
        this.canvas = canvas;
        this.x = 0;
        this.y = 0;
        this.down = false;
        this.up = false;
        this.click = false;
        this.move = false;
        this.mouseDown = this.mouseDown.bind(this);
        this.mouseUp = this.mouseUp.bind(this);
        this.mouseClick = this.mouseClick.bind(this);
        this.mouseMove = this.mouseMove.bind(this);
        this.canvas.addEventListener('mousedown', this.mouseDown);
        this.canvas.addEventListener('mouseup', this.mouseUp);
        this.canvas.addEventListener('click', this.mouseClick);
        this.canvas.addEventListener('mousemove', this.mouseMove);
    }

    mouseDown(e) {
        this.down = true;
        this.up = false;
    }

    mouseUp(e) {
        this.down = false;
        this.up = true;
    }

    mouseClick(e) {
        this.click = true;
    }

    mouseMove(e) {
        this.move = true;
        const rect = this.canvas.getBoundingClientRect();
        this.x = e.clientX - rect.left;
        this.y = e.clientY - rect.top;
    }

    update() {
        this.click = false;
        this.move = false;
    }

    destroy() {
        this.canvas.removeEventListener('mousedown', this.mouseDown);
        this.canvas.removeEventListener('mouseup', this.mouseUp);
        this.canvas.removeEventListener('click', this.mouseClick);
        this.canvas.removeEventListener('mousemove', this.mouseMove);
    }
}