let ECS = import("ecs");
let GameComps = import("../components");
let GameAssets = import("../Assets");

let Kitten = {
    create = |world| => {
        let kitten = ECS.Entity.create();

        let tr = ECS.Components.Transform.create();
        let rb = ECS.Components.Rigidbody.create();
        let sr = ECS.Components.SpriteRenderer.create(GameAssets.Sprites.Kitten);

        kitten->addComponent(tr);
        kitten->addComponent(sr);
        kitten->addComponent(rb);
        kitten->addComponent(ECS.Components.TileCollider.create());
        kitten->addComponent(ECS.Components.Collider.create());
        kitten->addComponent(GameComps.Collectible.create());

        tr.origin.x = 10;
        tr.origin.y = 19;

        kitten.Collectible.sounds.collected = GameAssets.Sounds.Collected;

        kitten->setTag("Kitten");

        return kitten;
    }
};

return Kitten;