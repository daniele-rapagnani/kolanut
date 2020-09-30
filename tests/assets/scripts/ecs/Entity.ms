let Entity = {
    create = => {
        let tag = null;
        return { 
            components = {}, 
            world = null,
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