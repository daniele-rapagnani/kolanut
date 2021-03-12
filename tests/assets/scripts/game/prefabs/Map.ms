
let Map = {
    create = |spawnFactories, world| => {
        return {
            spawnFactories = spawnFactories,
            world = world,
            tilemap = null
        } @ Map;
    },

    spawn = |factory, spawnPoint| -> {
        let ent = factory(this.world);
        ent.Transform.position.x = spawnPoint.x * 1.0;
        ent.Transform.position.y = spawnPoint.y * 1.0;
        
        this.world->addEntity(ent);

        return ent;
    },

    spawnByName = |name, spawnPoint| -> {
        if (!object.hasKey(this.spawnFactories, name)) {
            return null;
        }

        return this->spawn(this.spawnFactories[name], spawnPoint);
    },

    load = |folder, file| -> {
        this.tilemap = Kolanut.Tilemap.create();
        this.tilemap->load(folder, file);
        this.tilemap->instantiate(Vector2.create(0, 0));

        let playerSpawn = this.tilemap->getObjectByName(0, "Player Spawn");

        if (playerSpawn != null) {
            this->spawnByName("Hero", playerSpawn);
        }

        let spawns = this.tilemap->getObjectsByType(0, "Enemy Spawn");

        for (let spawnPoint in spawns) {
            if (spawnPoint.visible) {
                this->spawnByName(spawnPoint.name, spawnPoint);
            }
        }
    },

    draw = -> {
        this.tilemap->draw();
    },
};

return Map;