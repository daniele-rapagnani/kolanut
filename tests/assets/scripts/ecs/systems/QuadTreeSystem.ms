let System = import("./System");
let QuadTree = import("../QuadTree");
let World = import("../World");

let QuadTreeSystem = {
    create = => {
        return {
            quadTree = QuadTree.create(BoundingBox.create(700, 700, 1700, 1400))
        } @ QuadTreeSystem;
    },

    getQuadTree = -> {
        return this.quadTree;
    },

    update = |dt, entities, phase| -> {
        Kolanut.Tracy.beginZone("Quadtree update");
        this.quadTree->clear();

        for (let e in entities) {
            if (e.Camera != null) {
                continue;
            }

            let tr = e.Transform;

            if (tr == null) {
                continue;
            }

            if (tr.quadtreeDirty == false) {
                continue;
            }

            let bb = e->getBB();

            if (bb == null) {
                continue;
            }

            this.quadTree->insert(e, bb);
        }
        
        if (this.world.debug) {
            this.quadTree->visit(|node| => {
                let color = Color.yellow;

                if (#node.entries > 0) {
                    color = Color.magenta;
                }

                Kolanut.drawRect(
                    Rect.create(node.bb->getOrigin(), node.bb->getSize()), 
                    color
                );
            });
        }

        Kolanut.Tracy.endZone();
    }
} @ System.create("QuadTreeSystem");

return QuadTreeSystem;