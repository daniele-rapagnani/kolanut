let System = import("./System");

let CullingSystem = {
    create = |cameraTag| => {
        return { cameraTag = cameraTag } @ CullingSystem;
    },

    update = |dt, entities| -> {
        for (let e in entities) {
            e.inView = false;
        }

        let quadTreeSys = this.world->getSystem("QuadTreeSystem");

        if (quadTreeSys == null) {
            return;
        }

        let cameras = this.world->getTaggedEntities(this.cameraTag);

        if (#cameras == 0) {
            return;
        }

        let camera = cameras[0];

        let qt = quadTreeSys->getQuadTree();

        let ents = [];
        qt->query(camera->getBB() * 1.5, ents, camera);

        for (let entry in ents) {
            if (camera->getBB()->overlaps(entry.entity->getBB())) {
                entry.entity.inView = true;
            }
        }
    }
} @ System.create("CullingSystem");

return CullingSystem;