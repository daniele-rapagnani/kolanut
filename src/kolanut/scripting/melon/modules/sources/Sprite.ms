let Sprite = {
    create = |name| => {
        let s = { 
            pos = Vector.create(0.0, 0.0),
            angle = 0.0,
            scale = Vector.create(1.0, 1.0),
            texture = Kolanut.loadSprite(name)
        } @ Sprite;

        if (s.texture) {
            s.size = s.texture->getSize();
            s.rect = Rect.create(0.0, 0.0, s.size.x, s.size.y);
        }

        return s;
    },

    draw = -> {
        this.texture->draw(
            this.pos.x, this.pos.y,
            this.angle,
            this.scale.x, this.scale.y
        );
    }
};

return Sprite;