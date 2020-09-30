let System = import("./System");

let PlayerSystem = {
    create = => {
        return { } @ PlayerSystem;
    },

    update = |dt, entities| -> {
        let self = this;

        this.updateWithComponents(
            entities, 
            ["Player", "Collider"],
            |components, entity| => {
                let c = components.Collider;

                if (c.collidedWith != null) {
                    if (c.collidedWith.Enemy != null) {
                        if (c.collidedWith.FrameAnimation.curAnimation == "attack") {
                            self.world->removeEntity(entity);
                        }
                    }
                }
            }
        );
    }
} @ System.create();

return PlayerSystem;