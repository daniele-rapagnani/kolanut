let SpriteRenderer = import("ecs/components/SpriteRenderer");
let Transform = import("ecs/components/Transform");
let Rigidbody = import("ecs/components/Rigidbody");
let FrameAnimation = import("ecs/components/FrameAnimation");
let Camera = import("ecs/components/Camera");
let PanController = import("ecs/components/camera/PanController");

let SpriteRenderSystem = import("ecs/systems/SpriteRenderSystem");
let RigidbodySystem = import("ecs/systems/RigidbodySystem");
let FrameAnimationSystem = import("ecs/systems/FrameAnimationSystem");
let CameraSystem = import("ecs/systems/CameraSystem");
let PanControllerSystem = import("ecs/systems/PanControllerSystem");
let Entity = import("ecs/Entity");

let input = {
    left = false,
    right = false,
    up = false,
    down = false
};

let entities = [];

let systems = {
    update = [],
    draw = []
};

systems.update[] = PanControllerSystem.create();
systems.update[] = RigidbodySystem.create();
systems.update[] = FrameAnimationSystem.create();
systems.update[] = CameraSystem.create();
systems.draw[] = SpriteRenderSystem.create();

func makeCamera => {
    let camera = Entity.create();
    camera->addComponent(Transform.create());
    camera->addComponent(Camera.create());
    camera->addComponent(Rigidbody.create());
    camera->addComponent(PanController.create());

    return camera;
}

func makeGoblin => {
    let goblin = Entity.create();

    let tr = Transform.create();
    let rb = Rigidbody.create();
    let fa = FrameAnimation.create(Rect.create(0, 0, 16, 16));
    let sr = SpriteRenderer.create("assets/goblin_run_anim_strip_6.png");

    goblin->addComponent(tr);
    goblin->addComponent(sr);
    goblin->addComponent(fa);
    // goblin->addComponent(rb);
    
    fa.duration = 0.15;
    tr.scale = tr.scale * 3.0;
    rb.maxVelocity = Vector.create(200.0, 200.0);

    tr.origin.x = fa.frameSize.w * 0.50;
    tr.origin.y = fa.frameSize.h * 0.50;

    return goblin;
}

Kolanut.onKeyPressed = |key, pressed| => {
    input[key] = pressed;
};

let player = null;
let camera = null;

Kolanut.onLoad = => {
    camera = makeCamera();
    entities[] = camera;

    for (let i in 0..10) {
        let gob = makeGoblin();
        gob.Transform.position = Vector.create(
            Kolanut.getScreenSize().x * random.getRandomNumber(),
            Kolanut.getScreenSize().y * random.getRandomNumber()
        );

        entities[] = gob;
    }
};

Kolanut.onUpdate = |dt| => {
    camera.PanController.left = input.left;
    camera.PanController.right = input.right;
    camera.PanController.up = input.up;
    camera.PanController.down = input.down;

    for (let system in systems.update) {
        system->update(dt, entities);
    }
};

Kolanut.onDraw = => {
    for (let system in systems.draw) {
        system->update(0.0, entities);
    }
};