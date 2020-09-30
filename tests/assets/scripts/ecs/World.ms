let World = {
    create = => {
        return {
            systems = {
                update = [],
                draw = []
            },
            entities = [], 
            tagged = {},
            deletedQueue = []
        } @ World;
    },

    addSystem = |system, update| -> {
        let arr = null;

        if (update) {
            arr = this.systems.update;
        } else {
            arr = this.systems.draw;
        }

        system.world = this;
        arr[] = system;
    },

    addEntity = |entity| -> {
        this.entities[] = entity;
        entity.world = this;

        if (entity->getTag() != null) {
            this->setEntityTag(entity, entity->getTag());
        }
    },

    removeEntity = |entity| -> {
        if (entity.world == this) {
            this.deletedQueue[] = entity;
        }
    },

    removeFromArray = |entity, arr| => {
        if (arr != null && entity != null) {
            for (let i in #arr..0) {
                if (arr[i] == entity) {
                    array.delete(arr, i);
                }
            }
        }
    },

    setEntityTag = |entity, tag| -> {
        let oldTag = entity->getTag();

        if (oldTag != null) {
            if (object.hasKey(this.tagged, oldTag)) {
                World.removeFromArray(entity, this.tagged[oldTag]);
            }
        }

        if (tag != null) {
            this.tagged[tag] = (this.tagged[tag] ?? []);
            this.tagged[tag][] = entity;
        }
    },

    getTaggedEntities = |tag| -> {
        return this.tagged[tag] ?? [];
    },

    update = |dt| -> {
        for (let system in this.systems.update) {
            system->update(dt, this.entities);
        }
    },

    draw = -> {
        for (let system in this.systems.draw) {
            system->update(0.0, this.entities);
        }
    },

    cleanup = -> {
        if (#this.deletedQueue > 0) {
            for (let e in this.deletedQueue) {
                this->setEntityTag(e, null);
                World.removeFromArray(e, this.entities);
            }

            this.deletedQueue = [];
        }
    }
};

return World;