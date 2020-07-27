let SpriteRenderer = import("ecs/components/SpriteRenderer");
let Transform = import("ecs/components/Transform");
let Rigidbody = import("ecs/components/Rigidbody");
let FrameAnimation = import("ecs/components/FrameAnimation");
let Camera = import("ecs/components/Camera");
let PanController = import("ecs/components/camera/PanController");
let TileCollider = import("ecs/components/TileCollider");
let PlatformController = import("ecs/components/game/player/PlatformController");

let SpriteRenderSystem = import("ecs/systems/SpriteRenderSystem");
let RigidbodySystem = import("ecs/systems/RigidbodySystem");
let FrameAnimationSystem = import("ecs/systems/FrameAnimationSystem");
let CameraSystem = import("ecs/systems/CameraSystem");
let PanControllerSystem = import("ecs/systems/PanControllerSystem");
let TilemapCollisionSystem = import("ecs/systems/TilemapCollisionSystem");
let PlatformSystem = import("ecs/systems/PlatformSystem");
let Entity = import("ecs/Entity");

let input = {
    left = false,
    right = false,
    up = false,
    down = false
};

let player = null;
let camera = null;
let tilemap = null;

let entities = [];

let systems = {
    update = [],
    draw = []
};

systems.update[] = PanControllerSystem.create();
systems.update[] = PlatformSystem.create();
systems.update[] = RigidbodySystem.create();
systems.update[] = FrameAnimationSystem.create();
systems.update[] = CameraSystem.create();

let tilemapCollisionSystem = TilemapCollisionSystem.create();
systems.update[] = tilemapCollisionSystem;
systems.draw[] = SpriteRenderSystem.create();

func makeCamera => {
    let camera = Entity.create();
    camera->addComponent(Transform.create());
    camera->addComponent(Camera.create());
    camera->addComponent(PanController.create());

    camera.Transform.scale.x = 2.0;
    camera.Transform.origin = Kolanut.getScreenSize() * 0.5;

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
    // goblin->addComponent(rb);

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

    rb.maxVelocity = Vector.create(200.0, 200.0);

    tr.origin.x = 8;
    tr.origin.y = 16;

    return goblin;
}

let enemyFactories = {
    Goblin = makeGoblin
};

func spawn |factory, spawnPoint| => {
    let ent = factory();
    ent.Transform.position.x = spawnPoint.x;
    ent.Transform.position.y = spawnPoint.y;
    entities[] = ent;

    return ent;
}

func loadMap => {
    let tilemap = Kolanut.Tilemap.create();
    tilemap->load("assets/", "map.tmx");
    tilemap->instantiate(Vector.create(0, 0));

    let playerSpawn = tilemap->getObjectByName(0, "Player Spawn");

    if (playerSpawn != null) {
        player = spawn(makeHero, playerSpawn);
    }

    let spawns = tilemap->getObjectsByType(0, "Enemy Spawn");

    for (let spawnPoint in spawns) {
        spawn(enemyFactories[spawnPoint.name], spawnPoint);
    }

    return tilemap;
}

Kolanut.onKeyPressed = |key, pressed| => {
    input[key] = pressed;
};

Kolanut.onLoad = => {
    camera = makeCamera();
    entities[] = camera;

    tilemap = loadMap();
    tilemapCollisionSystem.tilemap = tilemap;
};

Kolanut.onUpdate = |dt| => {   
    player.PlatformController.controls.left = input.left;
    player.PlatformController.controls.right = input.right;
    player.PlatformController.controls.jump = input.up;

    camera.Transform.position = player.Transform.position;

    for (let system in systems.update) {
        system->update(dt, entities);
    }
};

Kolanut.onDraw = => {
    tilemap->draw();
    
    for (let system in systems.draw) {
        system->update(0.0, entities);
    }
};