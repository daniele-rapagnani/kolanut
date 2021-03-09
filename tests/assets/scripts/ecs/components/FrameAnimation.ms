let Component = import("./Component");

let FrameAnimation = {
    create = => {
        return { 
            animations = {},
            curAnimation = null,
            frame = 0,
            loop = true,
            time = 0.0,
            finishCallback = null
        } @ FrameAnimation;
    },

    addAnimation = |config| -> {
        let newConf = {
            duration = 1.0,
            totalFrames = null
        };

        object.merge(newConf, config);
        this.animations[config.name] = newConf;

        return true;
    },

    playAnimation = |name, loop| -> {
        if (this.curAnimation == null || this.curAnimation != name) {
            this->startAnimation(name, loop);
        }
    },
    
    startAnimation = |name, loop| -> {
        if (!object.hasKey(this.animations, name)) {
            return false;
        }

        this.curAnimation = name;
        this.loop = loop ?? true;
        this.time = 0.0;
        this.frame = 0;

        return true;
    },

    stopAnimation = -> {
        this.curAnimation = null;
    },

    getCurAnimation = -> {
        if (this.curAnimation == null) {
            return null;
        }

        return this.animations[this.curAnimation];
    },

    getSize = -> {
        let ca = this->getCurAnimation();

        if (ca == null) {
            return Vector2.create();
        }
        
        return ca.frameSize.size;
    },

    getType = -> {
        return "FrameAnimation";
    }
} @ Component.create();

return FrameAnimation;