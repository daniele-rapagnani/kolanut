let Component = import("./Component");

let SpriteRenderer = {
    create = |image| => {
        let s = { sprite = null } @ SpriteRenderer;
        s->load(image);
        return s;
    },

    load = |image| -> {
        this.sprite = Kolanut.loadSprite(image);

        if (this.sprite != null) {
            this.size = this.sprite->getSize();
            this.rect = Rect.create(0.0, 0.0, this.size.x, this.size.y);
        }
    },

    getType = -> {
        return "SpriteRenderer";
    }
} @ Component.create();

return SpriteRenderer;