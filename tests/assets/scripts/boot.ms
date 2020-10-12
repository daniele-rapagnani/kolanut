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

let uiState = {
    lives = { total = 3, count = 1 }
};

let updateUIState = => {
    let player = world->getTaggedEntities("Player");

    if (#player == 0) {
        return;
    }

    player = player[0];
    uiState.lives.count = player.Player.energy;
};

Kolanut.onDrawUI = => {
    world->drawUI();

    updateUIState();
    Game.UI.Lives.draw(Vector.create(10, 10), uiState.lives);
};