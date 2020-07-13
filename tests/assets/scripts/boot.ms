
let Sprite = {
    create = |name| => {
        return { 
            x = 0.0,
            y = 0.0,
            angle = 0.0,
            scaleX = 1.0,
            scaleY = 1.0,
            texture = kolanut.loadSprite(name)
        } @ Sprite;
    },

    draw = => {
        this.texture->draw(
            this.x, this.y,
            this.angle,
            this.scaleX, this.scaleY
        );
    }
};

let kitten = null;

kolanut.onLoad = => {
    io.print("Game loading");
    
    kitten = Sprite.create("assets/kitten.jpeg");
};

kolanut.onUpdate = |dt| => {
    kitten.x = kitten.x + 50.0 * dt;
    kitten.y = kitten.y + 70.0 * dt;
    kitten.angle = kitten.angle + 45.0 * dt;
    kitten.scaleX = kitten.scaleX + 0.4 * dt;
    kitten.scaleY = kitten.scaleX;
};

kolanut.onDraw = => {
    kitten->draw();
};