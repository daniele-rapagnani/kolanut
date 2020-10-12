let Utils = import("utils");

let World = {
    Phase = {
        Update = "update",
        Draw = "draw",
        DrawUI = "drawUI"
    },

    create = => {
        return {
            systems = {
                update = [],
                draw = [],
                drawUI = []
            },
            systemsMap = {},
            entities = [], 
            tagged = {},
            deletedQueue = [],

            oneShotFunctions = [],
            oneShotFunctionsDeleted = []
        } @ World;
    },

    clearEntities = -> {
        for (let e in this.entities) {
            this->removeEntity(e);
        }
    },

    clearSystems = -> {
        for (let k in this.systems) {
            for (let s in this.systems[k]) {
                s.world = null;
            }

            this.systems[k] = [];
        }

        this.systemsMap = {};
    },

    addSystem = |system, type| -> {
        if (system->getName() == null || #system->getName() == 0) {
            debug.error("Systems must have a name");
            return;
        }

        system.world = this;
        this.systems[type ?? World.Phase.Update][] = system;
        this.systemsMap[system->getName()] = system;
    },

    getSystem = |systemName| -> {
        return this.systemsMap[systemName];
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
            Utils.Array.delete(arr, entity);   
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

    updateOneShotFunctions = |dt| -> {
        for (let f in this.oneShotFunctions) {
            f.time = f.time + dt;

            if (f.time >= f.triggerTime) {
                f.function(f.time - f.triggerTime);
                this.oneShotFunctionsDeleted[] = f;
            }
        }
    },

    updateFunctions = |dt| -> {
        this->updateOneShotFunctions(dt);
    },

    scheduleFunction = |function, time, oneShot| -> {
        let f = {
            function = function,
            triggerTime = time * 1.0,
            time = 0.0
        };

        if (oneShot ?? true) {
            this.oneShotFunctions[] = f;
        }
    },

    update = |dt| -> {
        this->updateFunctions(dt);

        for (let system in this.systems.update) {
            if (!system.enabled) {
                continue;
            }

            system->update(dt, this.entities, World.Phase.Update);
        }
    },

    draw = -> {
        for (let system in this.systems.draw) {
            if (!system.enabled) {
                continue;
            }

            system->update(0.0, this.entities, World.Phase.Draw);
        }
    },

    drawUI = -> {
        for (let system in this.systems.drawUI) {
            if (!system.enabled) {
                continue;
            }
            
            system->update(0.0, this.entities, World.Phase.DrawUI);
        }
    },

    cleanup = -> {
        if (#this.deletedQueue > 0) {
            for (let e in this.deletedQueue) {
                this->setEntityTag(e, null);
                e.world = null;
                World.removeFromArray(e, this.entities);
            }

            this.deletedQueue = [];
        }

        if (#this.oneShotFunctionsDeleted > 0) {
            for (let fd in this.oneShotFunctionsDeleted) {
                Utils.Array.delete(this.oneShotFunctions, fd);
            }

            this.oneShotFunctionsDeleted = [];
        }
    }
};

return World;