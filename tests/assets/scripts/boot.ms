let SpriteRenderer = import("ecs/components/SpriteRenderer");
let Transform = import("ecs/components/Transform");
let Rigidbody = import("ecs/components/Rigidbody");

let SpriteRenderSystem = import("ecs/systems/SpriteRenderSystem");
let RigidbodySystem = import("ecs/systems/RigidbodySystem");
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
systems.draw[] = SpriteRenderSystem.create();

Kolanut.onKeyPressed = |key, pressed| => {
    input[key] = pressed;
};

let kitten = null;

Kolanut.onLoad = => {
    kitten = Entity.create();

    kitten->addComponent(Transform.create());
    kitten->addComponent(SpriteRenderer.create("assets/kitten.jpeg"));
    kitten->addComponent(Rigidbody.create());

    kitten->getComponent("Rigidbody").maxVelocity = Vector.create(200.0, 200.0);

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