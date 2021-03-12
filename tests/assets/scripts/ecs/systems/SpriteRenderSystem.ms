let System = import("./System");

let SpriteRenderSystem = {
    create = => {
        return { } @ SpriteRenderSystem;
    },

    update = |dt, entities| -> {
        this.updateWithComponents(
            entities, 
            ["Transform", "SpriteRenderer"],
            |components, ent| => {
                if (!ent->isInView()) {
                    return;
                }
                
                let tr = components.Transform;
                let sr = components.SpriteRenderer;

                sr.sprite->draw(
                    tr->getWorldTransform(),
                    sr.rect,
                    sr.color
                );
            }
        );
    }
} @ System.create("SpriteRenderSystem");

return SpriteRenderSystem;