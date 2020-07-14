let SpriteRenderer = import("ecs/components/SpriteRenderer");
let Transform = import("ecs/components/Transform");
let Rigidbody = import("ecs/components/Rigidbody");

let SpriteRenderSystem = import("ecs/systems/SpriteRenderSystem");
let RigidbodySystem = import("ecs/systems/RigidbodySystem");
let Entity = import("ecs/Entity");

let entities = [];

let systems = {
    update = [],
    draw = []
};

systems.update[] = RigidbodySystem.create();
systems.draw[] = SpriteRenderSystem.create();

Kolanut.onLoad = => {
    let kitten = Entity.create();

    kitten->addComponent(Transform.create());
    kitten->addComponent(SpriteRenderer.create("assets/kitten.jpeg"));
    kitten->addComponent(Rigidbody.create());

    kitten->getComponent("Rigidbody")->setForce(Vector.create(500.0, 200.0));

    entities[] = kitten;
};

Kolanut.onUpdate = |dt| => {
    for (let system in systems.update) {
        system->update(dt, entities);
    }
};

Kolanut.onDraw = => {
    for (let system in systems.draw) {
        system->update(0.0, entities);
    }
};