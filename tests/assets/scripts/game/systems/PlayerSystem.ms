let System = import("ecs/systems/System");

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
                            entity.PlatformController.enabled = false;
                            entity.Collider.enabled = false;
                            entity.Player.enabled = false;
                            entity.Rigidbody.enabled = false;

                            entity.FrameAnimation.finishCallback = |e| => {
                                self.world->removeEntity(e);
                                
                                if (entity.Player.onDeath != null) {
                                    entity.Player.onDeath();
                                }
                            };

                            entity.FrameAnimation->playAnimation("die", false);
                        }
                    }
                }
            }
        );
    }
} @ System.create();

return PlayerSystem;