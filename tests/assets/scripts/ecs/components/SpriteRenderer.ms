let SpriteRenderer = {
    create = |image| => {
        return { sprite = Kolanut.loadSprite(image) } @ SpriteRenderer;
    },

    getType = => {
        return "SpriteRenderer";
    }
};

return SpriteRenderer;