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

                sr.sprite->draw(
                    tr.position,
                    tr.angle, 
                    tr.scale,
                    tr.origin,
                    sr.rect,
                    sr.color
                );
            }
        );
    }
} @ System.create("SpriteRenderSystem");

return SpriteRenderSystem;