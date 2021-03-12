let Entity = {
    create = => {
        let tag = null;
        return { 
            components = {}, 
            world = null,
            inView = false,
            getTag = -> { return tag; },
            setTag = |newTag| -> { 
                if (this.world != null) {
                    this.world->setEntityTag(this, newTag);
                }
                
                tag = newTag;
            }
        } @ Entity;
    },

    addComponent = |component| -> {
        this.components[component->getType()] = component;
    },

    removeComponent = |component| -> {
        this.components[component->getType()] = null;
    },

    getComponent = |type| -> {
        return this.components[type];
    },

    isInView = -> {
        return this.inView;
    },

    getBB = func entityGetBB -> {
        let sr = this.SpriteRenderer;
        let fa = this.FrameAnimation;
        let ca = this.Camera;
        let tr = this.Transform;

        if (tr == null) {
            return null;
        }

        let size = null;

        if (fa != null) {
            size = fa->getSize();
        } else if (sr != null) {
            size = sr->getSize();
        } else if (ca != null) {
            return ca->getBB(tr);
        }

        if (size == null) {
            return null;
        }

        return tr->getBB(size);
    },

    [object.symbols.getPropertyOperator] = |prop| -> {
        if (!object.hasKey(Entity, prop) && !object.hasKey(this, prop)) {
            let v = this->getComponent(prop);
            
            if (v != null) {
                return v;
            }
        }

        return false;
    }
};

return Entity;