let System = import("./System");
let Collider = import("../components/Collider");
let Events = import("../events");

let CollisionSystem = {
    create = => {
        return { } @ CollisionSystem;
    },

    getRendererBB = |entity| => {
        let sr = entity.SpriteRenderer;
        let fa = entity.FrameAnimation;
        let tr = entity.Transform;

        if (tr == null) {
            return null;
        }

        let bb = null;

        if (fa != null) {
            bb = fa->getBB(tr);
        } else if (sr != null) {
            bb = sr->getBB(tr);
        }

        return bb;
    },

    update = |dt, entities| -> {
        for (let i in 0..#entities) {
            let a = entities[i].Collider;

            if (a == null) {
                continue;
            }
            
            for (let j in (i+1)..#entities) {
                let b = entities[j].Collider;

                if (b == null) {
                    continue;
                }

                if (a.type == Collider.Types.RendererBB && b.type == a.type) {
                    let aBB = CollisionSystem.getRendererBB(entities[i]);
                    let bBB = CollisionSystem.getRendererBB(entities[j]);

                    let overlapH = (aBB.left < bBB.right && aBB.right > bBB.left);
                    let overlapV = (aBB.bottom > bBB.top && aBB.top < bBB.bottom);

                    if (overlapH && overlapV) {
                        Events.Collision->trigger(this.world, entities[i], entities[j]);
                    }
                }
            }
        }
    }
} @ System.create("CollisionSystem");

return CollisionSystem;