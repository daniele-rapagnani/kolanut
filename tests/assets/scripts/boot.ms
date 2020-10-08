let SpriteRenderer = import("ecs/components/SpriteRenderer");
let Transform = import("ecs/components/Transform");
let Rigidbody = import("ecs/components/Rigidbody");
let FrameAnimation = import("ecs/components/FrameAnimation");
let Camera = import("ecs/components/Camera");
let PanController = import("ecs/components/camera/PanController");
let Collider = import("ecs/components/Collider");
let TileCollider = import("ecs/components/TileCollider");
let PlatformController = import("ecs/components/game/player/PlatformController");
let PatrolController = import("ecs/components/game/enemy/PatrolController");
let Player = import("ecs/components/game/player/Player");
let Enemy = import("ecs/components/game/enemy/Enemy");

let SpriteRenderSystem = import("ecs/systems/SpriteRenderSystem");
let RigidbodySystem = import("ecs/systems/RigidbodySystem");
let FrameAnimationSystem = import("ecs/systems/FrameAnimationSystem");
let CameraSystem = import("ecs/systems/CameraSystem");
let PanControllerSystem = import("ecs/systems/PanControllerSystem");
let TilemapCollisionSystem = import("ecs/systems/TilemapCollisionSystem");
let PlatformSystem = import("ecs/systems/PlatformSystem");
let PatrollingSystem = import("ecs/systems/PatrollingSystem");
let CollisionSystem = import("ecs/systems/CollisionSystem");
let PlayerSystem = import("ecs/systems/PlayerSystem");

let World = import("ecs/World");
let Entity = import("ecs/Entity");

let input = {
    left = false,
    right = false,
    up = false,
    down = false,
    space = false
};

let player = null;
let camera = null;
let tilemap = null;

let tilemapCollisionSystem = null;

let world = World.create();

func initSystems |world| => {
    world->addSystem(PanControllerSystem.create(), true);
    world->addSystem(PlatformSystem.create(), true);
    world->addSystem(PatrollingSystem.create("Player"), true);
    world->addSystem(RigidbodySystem.create(), true);
    world->addSystem(FrameAnimationSystem.create(), true);
    world->addSystem(CameraSystem.create(), true);

    world->addSystem(CollisionSystem.create(), true);
    world->addSystem(PlayerSystem.create(), true);
    tilemapCollisionSystem = TilemapCollisionSystem.create();
    world->addSystem(tilemapCollisionSystem, true);
    world->addSystem(SpriteRenderSystem.create(), false);
}

func makeCamera |world| => {
    let camera = Entity.create();
    camera->addComponent(Transform.create());
    camera->addComponent(Camera.create());
    camera->addComponent(PanController.create());

    camera.Transform.scale.x = 2.0;
    camera.Transform.origin = Kolanut.getScreenSize() * 0.5;
    camera.Camera.followTag = "Player";

    world->addEntity(camera);

    return camera;
}

func makeHero => {
    let hero = Entity.create();

    let tr = Transform.create();
    let fa = FrameAnimation.create();
    let rb = Rigidbody.create();
    let sr = SpriteRenderer.create("assets/herochar_spritesheet.png");

    hero->addComponent(tr);
    hero->addComponent(sr);
    hero->addComponent(rb);
    hero->addComponent(fa);
    hero->addComponent(TileCollider.create());
    hero->addComponent(PlatformController.create());
    hero->addComponent(Collider.create());
    hero->addComponent(Player.create());

    fa->addAnimation({
        name = "die",
        frameSize = Rect.create(0, 0, 16, 16),
        totalFrames = 8,
        duration = 0.15
    });

    fa->addAnimation({
        name = "idle",
        frameSize = Rect.create(0, 16 * 4, 16, 16),
        totalFrames = 4,
        duration = 0.15
    });

    fa->playAnimation("idle");

    tr.origin.x = 8;
    tr.origin.y = 10;

    hero->setTag("Player");

    return hero;
}

func makeGoblin => {
    let goblin = Entity.create();

    let tr = Transform.create();
    let rb = Rigidbody.create();
    let fa = FrameAnimation.create();
    let sr = SpriteRenderer.create("assets/goblin_spritesheet.png");

    goblin->addComponent(tr);
    goblin->addComponent(sr);
    goblin->addComponent(fa);
    goblin->addComponent(rb);
    goblin->addComponent(PatrolController.create());
    goblin->addComponent(TileCollider.create());
    goblin->addComponent(PlatformController.create());
    goblin->addComponent(Collider.create());
    goblin->addComponent(Enemy.create());

    goblin.PlatformController.speed = 15;
    goblin.PlatformController.dashingRecharge = 1.0;
    goblin.PlatformController.dashingSpeedMult = 5.0;
    goblin.PlatformController.animations.dash = "attack";

    fa->addAnimation({
        name = "run",
        frameSize = Rect.create(0, 0, 16, 16),
        totalFrames = 6,
        duration = 0.15
    });

    fa->addAnimation({
        name = "die",
        frameSize = Rect.create(0, 16, 16, 16),
        totalFrames = 6,
        duration = 0.15
    });

    fa->addAnimation({
        name = "attack",
        frameSize = Rect.create(0, 32, 24, 16),
        totalFrames = 4,
        duration = 0.1
    });

    fa->addAnimation({
        name = "idle",
        frameSize = Rect.create(0, 48, 16, 16),
        totalFrames = 4,
        duration = 0.15
    });

    fa->addAnimation({
        name = "hit",
        frameSize = Rect.create(0, 64, 16, 16),
        totalFrames = 3,
        duration = 0.15
    });

    fa->playAnimation("idle");

    tr.origin.x = 8;
    tr.origin.y = 16;

    goblin->setTag("Goblin");

    return goblin;
}

let enemyFactories = {
    Goblin = makeGoblin
};

func spawn |world, factory, spawnPoint| => {
    let ent = factory();
    ent.Transform.position.x = spawnPoint.x * 1.0;
    ent.Transform.position.y = spawnPoint.y * 1.0;
    
    world->addEntity(ent);

    return ent;
}

func loadMap |world| => {
    let tilemap = Kolanut.Tilemap.create();
    tilemap->load("assets/", "map.tmx");
    tilemap->instantiate(Vector.create(0, 0));

    let playerSpawn = tilemap->getObjectByName(0, "Player Spawn");

    if (playerSpawn != null) {
        player = spawn(world, makeHero, playerSpawn);
    }

    let spawns = tilemap->getObjectsByType(0, "Enemy Spawn");

    for (let spawnPoint in spawns) {
        spawn(world, enemyFactories[spawnPoint.name], spawnPoint);
    }

    return tilemap;
}

Kolanut.onKeyPressed = |key, pressed| => {
    input[key] = pressed;
};

let font = null;

Kolanut.onLoad = func kolanutOnLoad => {
    font = Kolanut.loadFont("assets/Pokemon Classic.ttf", 16);
    initSystems(world);

    camera = makeCamera(world);

    tilemap = loadMap(world);
    tilemapCollisionSystem.tilemap = tilemap;
};

Kolanut.onUpdate = |dt| => {   
    player.PlatformController.controls.left = input.left;
    player.PlatformController.controls.right = input.right;
    player.PlatformController.controls.jump = input.up;
    player.PlatformController.controls.dash = input.space;

    world->cleanup();
    world->update(dt);
};

Kolanut.onDraw = => {
    tilemap->draw();
    world->draw();
};

Kolanut.onDrawUI = => {
    font->draw("Hello darkness...\n...my old friend", Vector.create(0, 0), null, Color.green);
};