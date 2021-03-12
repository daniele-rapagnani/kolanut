let System = import("./System");
let Collider = import("../components/Collider");
let Events = import("../events");

let CollisionSystem = {
    create = => {
        return { } @ CollisionSystem;
    },

    update = |dt, entities| -> {
        let qts = this.world->getSystem("QuadTreeSystem");
        let qt = qts->getQuadTree();

        for (let e in entities) {
            if (e.Collider != null) {
                e.Collider.collided = false;
            }
        }

        for (let i in 0..#entities) {
            if (!entities[i]->isInView()) {
                continue;
            }

            let a = entities[i].Collider;

            if (a == null) {
                continue;
            }

            let res = [];
            let thisBB = entities[i]->getBB();
            qt->query(thisBB, res, entities[i]);

            for (let e in res) {
                let b = e.entity.Collider;

                if (b == null) {
                    continue;
                }

                if (b.collided) {
                    continue;
                }

                if (a.type == Collider.Types.RendererBB && b.type == a.type) {
                    let bBB = e.entity->getBB();

                    a.collided = true;
                    b.collided = true;

                    if (bBB != null && thisBB->overlaps(bBB)) {
                        Events.Collision->trigger(this.world, entities[i], e.entity);
                    }
                }
            }
        }
    }
} @ System.create("CollisionSystem");

return CollisionSystem;