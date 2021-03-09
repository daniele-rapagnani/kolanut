let Component = import("./Component");

let SpriteRenderer = {
    create = |image| => {
        let s = { sprite = null, color = Color.white } @ SpriteRenderer;
        s->load(image);
        return s;
    },

    load = |image| -> {
        if (types.isString(image)) {
            this.sprite = Kolanut.loadSprite(image);
        } else {
            this.sprite = image;
        }

        if (this.sprite != null) {
            this.size = this.sprite->getSize();
            this.rect = Rect.create(Vector2.create(), this.size);
        }
    },

    getSize = -> {
        if (this.sprite == null) {
            return Vector2.create();
        }

        return this.sprite->getSize();
    },

    getType = -> {
        return "SpriteRenderer";
    }
} @ Component.create();

return SpriteRenderer;