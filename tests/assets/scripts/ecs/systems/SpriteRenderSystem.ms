let System = import("./System");

let SpriteRenderSystem = {
    create = => {
        return { } @ SpriteRenderSystem;
    },

    update = |dt, entities| => {
        this.updateWithComponents(
            entities, 
            ["Transform", "SpriteRenderer"],
            |components| => {
                let tr = components.Transform;
                components.SpriteRenderer.sprite->draw(
                    tr.position.x,
                    tr.position.y,
                    tr.angle, 
                    tr.scale.x,
                    tr.scale.y
                );
            }
        );
    }
} @ System;

return SpriteRenderSystem;