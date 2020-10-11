let ECS = import("ecs");

let Map = import("./Map");
let Goblin = import("./Goblin");
let Hero = import("./Hero");

let ParentMap = Map.create({
    Hero = Hero.create,
    Goblin = Goblin.create
});

let ForestMap = {
    create = |world| => {
        let inst = { world = world } @ ForestMap;
        inst->load("assets/", "map.tmx");

        let fm = ECS.Entity.create();
        fm->addComponent(ECS.Components.Tilemap.create(inst.tilemap));
        fm->setTag("Tilemap");

        world->addEntity(fm);

        return fm;
    }
} @ ParentMap;

return ForestMap;