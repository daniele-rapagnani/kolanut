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
            this.rect = Rect.create(0.0, 0.0, this.size.x, this.size.y);
        }
    },

    getBB = |tr| -> {
        let res = {
            left = tr.position.x - tr.origin.x,
            top = tr.position.y - tr.origin.y
        };

        res.right = res.left + (this.sprite->getSize().x ?? 0.0);
        res.bottom = res.top + (this.sprite->getSize().y ?? 0.0);

        return res;
    },

    getType = -> {
        return "SpriteRenderer";
    }
} @ Component.create();

return SpriteRenderer;