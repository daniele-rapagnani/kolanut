let Scene = {
    create = |world, initialPrefabs| => {
        let inst = { world = world, initialPrefabs = initialPrefabs } @ Scene;
        return inst;
    },

    initSystems = -> { },

    init = -> { },

    load = -> {
        this->initSystems();
        this->addPrefabs(this.initialPrefabs);
        this->init();
    },

    reload = -> {
        this.world->clearSystems();
        this.world->clearEntities();
        this.world->cleanup();
        this->load();
    },

    addPrefabs = |prefabs| -> {
        for (let p in prefabs) {
            p.create(this.world);
        }
    },

    update = |dt| -> { },
    onKeyPressed = |key, pressed| -> { }
};

return Scene;