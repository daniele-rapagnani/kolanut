let ECS = import("ecs");
let GameComps = import("../components");
let GameAssets = import("../Assets");

let Goblin = {
    create = => {
        let goblin = ECS.Entity.create();

        let tr = ECS.Components.Transform.create();
        let rb = ECS.Components.Rigidbody.create();
        let fa = ECS.Components.FrameAnimation.create();
        let sr = ECS.Components.SpriteRenderer.create(GameAssets.Sprites.Goblin);

        goblin->addComponent(tr);
        goblin->addComponent(sr);
        goblin->addComponent(fa);
        goblin->addComponent(rb);
        goblin->addComponent(GameComps.PatrolController.create());
        goblin->addComponent(ECS.Components.TileCollider.create());
        goblin->addComponent(GameComps.PlatformController.create());
        goblin->addComponent(ECS.Components.Collider.create());
        goblin->addComponent(GameComps.Enemy.create());

        goblin.PlatformController.speed = 15;
        goblin.PlatformController.dashingRecharge = 1.0;
        goblin.PlatformController.dashingSpeedMult = 5.0;
        goblin.PlatformController.animations.dash = "attack";
        goblin.PlatformController.sounds.dash = GameAssets.Sounds.Attack;

        fa->addAnimation({
            name = "run",
            frameSize = Rect.create(Vector2.create(), Vector2.create(16, 16)),
            totalFrames = 6,
            duration = 0.15
        });

        fa->addAnimation({
            name = "die",
            frameSize = Rect.create(Vector2.create(0, 16), Vector2.create(16, 16)),
            totalFrames = 6,
            duration = 0.15
        });

        fa->addAnimation({
            name = "attack",
            frameSize = Rect.create(Vector2.create(0, 32), Vector2.create(24, 16)),
            totalFrames = 4,
            duration = 0.1
        });

        fa->addAnimation({
            name = "idle",
            frameSize = Rect.create(Vector2.create(0, 48), Vector2.create(16, 16)),
            totalFrames = 4,
            duration = 0.15
        });

        fa->addAnimation({
            name = "hit",
            frameSize = Rect.create(Vector2.create(0, 64), Vector2.create(16, 16)),
            totalFrames = 3,
            duration = 0.15
        });

        fa->playAnimation("idle");

        tr.origin.x = 8;
        tr.origin.y = 16;

        goblin->setTag("Goblin");

        return goblin;
    }
};

return Goblin;