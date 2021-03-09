let System = import("./System");

let PanControllerSystem = {
    create = => {
        return { } @ PanControllerSystem;
    },

    update = |dt, entities| -> {
        this.updateWithComponents(
            entities,
            ["Rigidbody", "PanController"],
            |components| => {
                let pc = components.PanController;
                let rb = components.Rigidbody;

                if (pc.left) {
                    rb->setForce(Vector2.create(-pc.speed, 0.0));
                } else if (pc.right) {
                    rb->setForce(Vector2.create(pc.speed, 0.0));
                }
                
                if (pc.down) {
                    rb->setForce(Vector2.create(0.0, pc.speed));
                } else if (pc.up) {
                    rb->setForce(Vector2.create(0.0, -pc.speed));
                }

                if (!pc.left && !pc.right && !pc.up && !pc.down) {
                    rb->setForce(Vector2.create(0.0, 0.0));
                }
            }
        );
    }
} @ System.create("PanControllerSystem");

return PanControllerSystem;