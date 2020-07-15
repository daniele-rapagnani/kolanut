let Component = import("./Component");

let SpriteRenderer = {
    create = |image| => {
        let s = { sprite = Kolanut.loadSprite(image) } @ SpriteRenderer;

        if (s.sprite != null) {
            s.size = s.sprite->getSize();
            s.rect = Rect.create(0.0, 0.0, s.size.x, s.size.y);
        }

        return s;
    },

    getType = => {
        return "SpriteRenderer";
    }
} @ Component.create();

return SpriteRenderer;