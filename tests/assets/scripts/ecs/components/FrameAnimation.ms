let Component = import("./Component");

let FrameAnimation = {
    create = |frameSize| => {
        return { 
            frame = 0,
            totalFrames = null,
            loop = true,
            frameSize = frameSize,
            duration = 1.0,
            time = 0.0 
        } @ FrameAnimation;
    },

    getType = -> {
        return "FrameAnimation";
    }
} @ Component.create();

return FrameAnimation;