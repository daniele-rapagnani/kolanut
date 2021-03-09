let System = import("./System");

let FrameAnimationSystem = {
    create = => {
        return { } @ FrameAnimationSystem;
    },

    update = |dt, entities| -> {
        this.updateWithComponents(
            entities,
            ["SpriteRenderer", "FrameAnimation"],
            |components, entity| => {
                let sr = components.SpriteRenderer;
                let fa = components.FrameAnimation;

                if (fa->getCurAnimation() != null) {
                    let ca = fa->getCurAnimation();

                    if (ca.totalFrames == null) {
                        ca.totalFrames = math.round(sr.size.x / ca.frameSize.size.x);
                    }

                    fa.time = fa.time + dt;
                    
                    while (fa.time >= ca.duration) {
                        fa.time = fa.time - ca.duration;

                        if (fa.frame + 1 >= ca.totalFrames && !fa.loop) {
                            fa->stopAnimation();

                            if (fa.finishCallback != null) {
                                fa.finishCallback(entity);
                            }

                            break;
                        }

                        fa.frame = (fa.frame + 1) % ca.totalFrames;
                    }

                    sr.rect.origin.y = ca.frameSize.origin.y;
                    sr.rect.origin.x = ca.frameSize.origin.x + (ca.frameSize.size.x * fa.frame);
                    sr.rect.size.x = ca.frameSize.size.x;
                    sr.rect.size.y = ca.frameSize.size.y;
                }
            }
        );
    }
} @ System.create("FrameAnimationSystem");

return FrameAnimationSystem;