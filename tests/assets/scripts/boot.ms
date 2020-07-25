let SpriteRenderer = import("ecs/components/SpriteRenderer");
let Transform = import("ecs/components/Transform");
let Rigidbody = import("ecs/components/Rigidbody");
let FrameAnimation = import("ecs/components/FrameAnimation");
let Camera = import("ecs/components/Camera");

let SpriteRenderSystem = import("ecs/systems/SpriteRenderSystem");
let RigidbodySystem = import("ecs/systems/RigidbodySystem");
let FrameAnimationSystem = import("ecs/systems/FrameAnimationSystem");
let CameraSystem = import("ecs/systems/CameraSystem");
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

systems.update[] = RigidbodySystem.create();
systems.update[] = FrameAnimationSystem.create();
systems.update[] = CameraSystem.create();
systems.draw[] = SpriteRenderSystem.create();

func makeCamera => {
    let camera = Entity.create();
    camera->addComponent(Transform.create());
    camera->addComponent(Camera.create());

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
    goblin->addComponent(rb);
    
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
    // if (input.left) {
    //     player.Rigidbody->setForce(Vector.create(-500.0, 0.0));
    // }
    // else if (input.right) {
    //     player.Rigidbody->setForce(Vector.create(500.0, 0.0));
    // }
    
    // if (input.down) {
    //     player.Rigidbody->setForce(Vector.create(0.0, 500.0));
    // }
    // else if (input.up) {
    //     player.Rigidbody->setForce(Vector.create(0.0, -500.0));
    // }

    // if (!input.left && !input.right && !input.up && !input.down) {
    //     player.Rigidbody->setForce(Vector.create(0.0, 0.0));
    // }

    // player.Transform.scale =
    //     player.Transform.scale * 1.01
    // ;

    // player.Transform.angle =
    //     player.Transform.angle + 90.0 * dt
    // ;

    camera.Transform.position = camera.Transform.position + Vector.create(50, 0) * dt;
    camera.Transform.scale.x = camera.Transform.scale.x * 1.001; 

    for (let system in systems.update) {
        system->update(dt, entities);
    }
};

Kolanut.onDraw = => {
    for (let system in systems.draw) {
        system->update(0.0, entities);
    }
};