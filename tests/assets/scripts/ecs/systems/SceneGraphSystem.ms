let System = import("./System");

let SceneGraphSystem = {
    create = => {
        return { } @ SceneGraphSystem;
    },

    update = |dt, entities| -> {
        func updateTransform |transform| => {
            if (transform.dirty) {
                if (transform.parent == null) {
                    transform.worldPosition = transform.position;
                    transform.worldAngle = transform.angle;
                    transform.worldScale = transform.scale;
                    
                    // return;
                } else {
                    updateTransform(transform.parent);

                    transform.worldPosition = 
                        transform.parent.worldPosition + transform.position
                        - transform.origin
                    ;
                    transform.worldAngle = transform.parent.worldAngle + transform.angle;
                    transform.worldScale = transform.scale;
                    transform.origin = transform.position;
                }

                transform.dirty = false;
            }
        }

        this.updateWithComponents(
            entities,
            ["Transform"],
            |components| => {
                updateTransform(components.Transform);
            }
        );
    }
} @ System.create();

return SceneGraphSystem;