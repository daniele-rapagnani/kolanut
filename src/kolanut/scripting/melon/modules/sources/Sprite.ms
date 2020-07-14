return {
    create = |name| => {
        return { 
            pos = Vector.create(0.0, 0.0),
            angle = 0.0,
            scale = Vector.create(1.0, 1.0),
            texture = Kolanut.loadSprite(name)
        } @ Sprite;
    },

    draw = => {
        this.texture->draw(
            this.pos.x, this.pos.y,
            this.angle,
            this.scale.x, this.scale.y
        );
    }
};