let System = import("./System");

let CameraSystem = {
    create = => {
        return { } @ CameraSystem;
    },

    update = |dt, entities| -> {
        this.updateWithComponents(
            entities,
            ["Transform", "Camera"],
            |components| => {
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