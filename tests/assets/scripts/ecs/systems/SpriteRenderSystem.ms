let System = import("./System");

let SpriteRenderSystem = {
    create = => {
        return { } @ SpriteRenderSystem;
    },

    update = |dt, entities| -> {
        this.updateWithComponents(
            entities, 
            ["Transform", "SpriteRenderer"],
            |components| => {
                let tr = components.Transform;
                let sr = components.SpriteRenderer;

                sr.sprite->drawRect(
                    tr.position,
                    tr.angle, 
                    tr.scale,
                    tr.origin,
                    sr.rect
                );
            }
        );
    }
} @ System.create();

return SpriteRenderSystem;