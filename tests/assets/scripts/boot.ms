let SpriteRenderer = import("ecs/components/SpriteRenderer");
let Transform = import("ecs/components/Transform");
let Rigidbody = import("ecs/components/Rigidbody");
let FrameAnimation = import("ecs/components/FrameAnimation");

let SpriteRenderSystem = import("ecs/systems/SpriteRenderSystem");
let RigidbodySystem = import("ecs/systems/RigidbodySystem");
let FrameAnimationSystem = import("ecs/systems/FrameAnimationSystem");
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
systems.draw[] = SpriteRenderSystem.create();

let kitten = null;

Kolanut.onKeyPressed = |key, pressed| => {
    input[key] = pressed;
};

Kolanut.onLoad = => {
    let ss = Kolanut.getScreenSize();

    kitten = Entity.create();

    let tr = Transform.create();
    let rb = Rigidbody.create();
    let fa = FrameAnimation.create(Rect.create(0, 0, 16, 16));

    kitten->addComponent(tr);
    kitten->addComponent(SpriteRenderer.create("assets/goblin_run_anim_strip_6.png"));
    kitten->addComponent(fa);
    kitten->addComponent(rb);
    
    fa.duration = 0.15;
    tr.scale = tr.scale * 3.0;
    rb.maxVelocity = Vector.create(200.0, 200.0);

    entities[] = kitten;
};

Kolanut.onUpdate = |dt| => {
    if (input.left) {
        kitten->getComponent("Rigidbody")->setForce(Vector.create(-500.0, 0.0));
    }
    else if (input.right) {
        kitten->getComponent("Rigidbody")->setForce(Vector.create(500.0, 0.0));
    }
    
    if (input.down) {
        kitten->getComponent("Rigidbody")->setForce(Vector.create(0.0, 500.0));
    }
    else if (input.up) {
        kitten->getComponent("Rigidbody")->setForce(Vector.create(0.0, -500.0));
    }

    if (!input.left && !input.right && !input.up && !input.down) {
        kitten->getComponent("Rigidbody")->setForce(Vector.create(0.0, 0.0));
    }

    for (let system in systems.update) {
        system->update(dt, entities);
    }
};

Kolanut.onDraw = => {
    for (let system in systems.draw) {
        system->update(0.0, entities);
    }
};