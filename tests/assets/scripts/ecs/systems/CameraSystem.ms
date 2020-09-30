let System = import("./System");

let CameraSystem = {
    create = => {
        return { } @ CameraSystem;
    },

    update = |dt, entities| -> {
        let self = this;

        this.updateWithComponents(
            entities,
            ["Transform", "Camera"],
            |components| => {
                let targets = self.world->getTaggedEntities(components.Camera.followTag);

                if (#targets > 0) {
                    components.Transform.position = targets[0].Transform.position;
                }

                Kolanut.Camera.setPosition(
                    components.Transform.position
                    - components.Transform.origin * (1.0 / components.Transform.scale.x)
                );
                Kolanut.Camera.setZoom(components.Transform.scale.x);
            }
        );
    }
} @ System.create();

return CameraSystem;