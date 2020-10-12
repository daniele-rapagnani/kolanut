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
                let tr = components.Transform;
                let ca = components.Camera;

                let targets = self.world->getTaggedEntities(ca.followTag);

                if (#targets > 0) {
                    tr.position = targets[0].Transform.position;
                }

                let pos = tr.position - tr.origin * (1.0 / tr.scale.x);
                let bb = ca->getBB(tr);

                if (ca.bounds.bottom != null && bb.bottom > ca.bounds.bottom) {
                    pos.y = pos.y - (bb.bottom - ca.bounds.bottom);
                }

                Kolanut.Camera.setPosition(pos);
                Kolanut.Camera.setZoom(components.Transform.scale.x);
            }
        );
    }
} @ System.create("CameraSystem");

return CameraSystem;