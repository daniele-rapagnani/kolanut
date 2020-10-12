let ECS = import("ecs");
let GameComps = import("../components");
let GameAssets = import("../Assets");

let Hero = {
    create = |world| => {
        let hero = ECS.Entity.create();

        let tr = ECS.Components.Transform.create();
        let fa = ECS.Components.FrameAnimation.create();
        let rb = ECS.Components.Rigidbody.create();
        let sr = ECS.Components.SpriteRenderer.create(GameAssets.Sprites.Hero);

        hero->addComponent(tr);
        hero->addComponent(sr);
        hero->addComponent(rb);
        hero->addComponent(fa);
        hero->addComponent(ECS.Components.TileCollider.create());
        hero->addComponent(GameComps.PlatformController.create());
        hero->addComponent(ECS.Components.Collider.create());
        hero->addComponent(GameComps.Player.create());

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
        
        hero.Player.onDeath = => {
            let gameOver = world->getTaggedEntities("Game Over Label")[0];
            gameOver.Label.enabled = true;

            world->scheduleFunction(=> {
                world->clearEntities();
                world.scene->reload();
            }, 2.0);
        };

        return hero;
    }
};

return Hero;