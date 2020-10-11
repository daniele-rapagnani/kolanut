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
                        ca.totalFrames = math.round(sr.size.x / ca.frameSize.w);
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

                    sr.rect.y = ca.frameSize.y;
                    sr.rect.x = ca.frameSize.x + (ca.frameSize.w * fa.frame);
                    sr.rect.w = ca.frameSize.w;
                    sr.rect.h = ca.frameSize.h;
                }
            }
        );
    }
} @ System.create();

return FrameAnimationSystem;