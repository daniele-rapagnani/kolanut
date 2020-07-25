let System = import("./System");

let FrameAnimationSystem = {
    create = => {
        return { } @ FrameAnimationSystem;
    },

    update = |dt, entities| -> {
        this.updateWithComponents(
            entities,
            ["SpriteRenderer", "FrameAnimation"],
            |components| => {
                let sr = components.SpriteRenderer;
                let fa = components.FrameAnimation;

                if (fa.totalFrames == null) {
                    fa.totalFrames = math.round(sr.size.x / fa.frameSize.w);
                }

                fa.time = fa.time + dt;

                while (fa.time >= fa.duration) {
                    fa.time = fa.time - fa.duration;
                    fa.frame = (fa.frame + 1) % fa.totalFrames;
                }

                sr.rect.x = fa.frameSize.w * fa.frame;
                sr.rect.w = fa.frameSize.w;
            }
        );
    }
} @ System.create();

return FrameAnimationSystem;