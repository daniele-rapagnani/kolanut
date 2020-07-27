let System = import("./System");

let PlatformSystem = {
    create = => {
        return { } @ PlatformSystem;
    },

    update = |dt, entities| -> {
        this.updateWithComponents(
            entities,
            ["Transform", "PlatformController", "TileCollider", "FrameAnimation", "Rigidbody"],
            |components| => {
                let pc = components.PlatformController;
                let fa = components.FrameAnimation;
                let rb = components.Rigidbody;
                let tr = components.Transform;
                let tc = components.TileCollider;

                if (pc.controls.left) {
                    rb.velocity.x = -pc.speed;
                    tr.scale.x = -1.0;
                } else if (pc.controls.right) {
                    rb.velocity.x = pc.speed;
                    tr.scale.x = 1.0;
                }

                if (!pc.controls.left && !pc.controls.right) {
                    fa->playAnimation(pc.animations.idle);
                    rb.velocity.x = 0;
                } else {
                    fa->playAnimation(pc.animations.run);
                }

                if (pc.controls.jump && tc.colliding.bottom) {
                    pc.controls.jump = false;
                    rb.velocity.y = rb.velocity.y - pc.jumpSpeed;
                }
            }
        );
    }
} @ System.create();

return PlatformSystem;