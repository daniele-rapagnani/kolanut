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
            |components, e| => {
                let tr = components.Transform;
                let ca = components.Camera;

                let targets = self.world->getTaggedEntities(ca.followTag);

                if (#targets > 0) {
                    tr.position.x = targets[0].Transform.position.x;
                    tr.position.y = targets[0].Transform.position.y;
                }

                let zoom = components.Transform.scale.x;
                let bb = ca->getBB(tr);

                if (ca.bounds.bottom != null && bb.bottom > ca.bounds.bottom) {
                    tr.position.y = tr.position.y - (bb.bottom - ca.bounds.bottom) * zoom;
                }

                Kolanut.Camera.set(tr.position, zoom, tr.origin);
            }
        );
    }
} @ System.create("CameraSystem");

return CameraSystem;