let Entity = {
    create = => {
        return { components = {} } @ Entity;
    },

    addComponent = |component| => {
        this.components[component.getType()] = component;
    },

    removeComponent = |component| => {
        this.components[component.getType()] = null;
    },

    getComponent = |type| => {
        return this.components[type];
    } 
};

return Entity;