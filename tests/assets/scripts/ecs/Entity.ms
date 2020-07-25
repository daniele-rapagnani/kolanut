let Entity = {
    create = => {
        return { components = {} } @ Entity;
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
            return this->getComponent(prop);
        }

        return false;
    }
};

return Entity;