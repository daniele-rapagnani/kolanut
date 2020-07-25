let SpriteRenderer = import("ecs/components/SpriteRenderer");
let Transform = import("ecs/components/Transform");
let Rigidbody = import("ecs/components/Rigidbody");
let FrameAnimation = import("ecs/components/FrameAnimation");

let SpriteRenderSystem = import("ecs/systems/SpriteRenderSystem");
let RigidbodySystem = import("ecs/systems/RigidbodySystem");
let FrameAnimationSystem = import("ecs/systems/FrameAnimationSystem");
let SceneGraphSystem = import("ecs/systems/SceneGraphSystem");
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
systems.update[] = SceneGraphSystem.create();
systems.update[] = FrameAnimationSystem.create();
systems.draw[] = SpriteRenderSystem.create();

func makeGoblin => {
    let goblin = Entity.create();

    let tr = Transform.create();
    let rb = Rigidbody.create();
    let fa = FrameAnimation.create(Rect.create(0, 0, 16, 16));
    let sr = SpriteRenderer.create("assets/red_square.png");

    goblin->addComponent(tr);
    // goblin->addComponent(SpriteRenderer.create("assets/goblin_run_anim_strip_6.png"));
    goblin->addComponent(sr);
    // goblin->addComponent(fa);
    goblin->addComponent(rb);
    
    fa.duration = 0.15;
    // tr.scale = tr.scale * 3.0;
    rb.maxVelocity = Vector.create(200.0, 200.0);

    // tr.origin.x = fa.frameSize.w * 0.50;
    // tr.origin.y = fa.frameSize.h * 0.50;
    tr.origin.x = sr.sprite->getSize().x * 0.50;
    tr.origin.y = sr.sprite->getSize().y * 0.50;

    return goblin;
}

Kolanut.onKeyPressed = |key, pressed| => {
    input[key] = pressed;
};

let player = null;

Kolanut.onLoad = => {
    let g1 = makeGoblin();
    let g2 = makeGoblin();

    g2.Transform.parent = g1.Transform;
    
    let sr = Kolanut.getScreenSize();
    g1.Transform.position = Vector.create(sr.x * 0.5, sr.y * 0.5);
    let g1s = g1.SpriteRenderer.sprite->getSize();
    g2.Transform.position = g1s;

    entities[] = g1;
    entities[] = g2;

    player = g1;
};

Kolanut.onUpdate = |dt| => {
    if (input.left) {
        player.Rigidbody->setForce(Vector.create(-500.0, 0.0));
    }
    else if (input.right) {
        player.Rigidbody->setForce(Vector.create(500.0, 0.0));
    }
    
    if (input.down) {
        player.Rigidbody->setForce(Vector.create(0.0, 500.0));
    }
    else if (input.up) {
        player.Rigidbody->setForce(Vector.create(0.0, -500.0));
    }

    if (!input.left && !input.right && !input.up && !input.down) {
        player.Rigidbody->setForce(Vector.create(0.0, 0.0));
    }

    // player->getComponent("Transform").scale =
    //     player->getComponent("Transform").scale * 1.01
    // ;

    player.Transform.angle =
        player.Transform.angle + 90.0 * dt
    ;

    for (let system in systems.update) {
        system->update(dt, entities);
    }
};

Kolanut.onDraw = => {
    for (let system in systems.draw) {
        system->update(0.0, entities);
    }
};