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
                Kolanut.Camera.setPosition(components.Transform.position);
                Kolanut.Camera.setZoom(components.Transform.scale.x);
            }
        );
    }
} @ System.create();

return CameraSystem;