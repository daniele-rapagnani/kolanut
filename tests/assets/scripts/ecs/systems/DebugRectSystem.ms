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

            if (tr == null) {
                continue;
            }

            let size = null;

            if (fa != null) {
                size = fa->getSize();
            } else if (sr != null) {
                size = sr->getSize();
            }

            if (size == null) {
                continue;
            }

            let bb = tr->getBB(size);

            r.origin.x = bb.left;
            r.origin.y = bb.top;
            r.size.x = bb.right - bb.left;
            r.size.y = bb.bottom - bb.top;

            Kolanut.drawRect(r, Color.red);
        }
    }
} @ System.create("DebugRectSystem");

return DebugRectSystem;