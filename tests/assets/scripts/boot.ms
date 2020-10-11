let ECS = import("ecs");
let Game = import("game");

let world = ECS.World.create();

Kolanut.onKeyPressed = |key, pressed| => {
    world.scene->onKeyPressed(key, pressed);
};

Kolanut.onLoad = func kolanutOnLoad => {
    Game.Assets.load();
    world.scene = Game.Scenes.Game.create(world);
    world.scene->load();
};

Kolanut.onUpdate = |dt| => {
    world.scene->update(dt);
    world->cleanup();
    world->update(dt);
};

Kolanut.onDraw = => {
    world->draw();
};

Kolanut.onDrawUI = => {
    world->drawUI();
};