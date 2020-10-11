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

    getBB = |tr| -> {
        let ca = this->getCurAnimation();

        let res = {
            left = tr.position.x - tr.origin.x,
            top = tr.position.y - tr.origin.y,
        };

        if (ca == null) {
            res.right = res.left;
            res.bottom = res.top;
            return res;
        }

        res.right = res.left + ca.frameSize.w;
        res.bottom = res.top + ca.frameSize.h;

        return res;
    },

    getType = -> {
        return "FrameAnimation";
    }
} @ Component.create();

return FrameAnimation;