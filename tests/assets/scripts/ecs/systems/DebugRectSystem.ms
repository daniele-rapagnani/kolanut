let System = import("./System");

let DebugRectSystem = {
    create = => {
        return { } @ DebugRectSystem;
    },

    update = |dt, entities| -> {
        if (!this.world.debug) {
            return;
        }

        let r = Rect.create();

        for (let e in entities) {
            let tr = e.Transform;
            let sr = e.SpriteRenderer;
            let fa = e.FrameAnimation;
            let bb = null;

            if (tr == null) {
                continue;
            }

            if (fa != null) {
                bb = fa->getBB(tr);
            } else if (sr != null) {
                bb = sr->getBB(tr);
            }

            if (bb == null) {
                continue;
            }

            r.origin.x = bb.left;
            r.origin.y = bb.top;
            r.size.x = bb.right - bb.left;
            r.size.y = bb.bottom - bb.top;

            Kolanut.drawRect(r, Color.red);
        }
    }
} @ System.create("DebugRectSystem");

return DebugRectSystem;