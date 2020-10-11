let ECS = import("ecs");
let Prefabs = import("../prefabs");
let Systems = import("../systems");

let Game = {
    create = |world| => { 
        let SuperClass = Game @ ECS.Scene.create(
            world, 
            [
                Prefabs.Camera,
                Prefabs.ForestMap,
                Prefabs.GameOverLabel
            ]
        );

        return {
            player = null,
            input = {
                left = false,
                right = false,
                up = false,
                down = false,
                space = false
            }
        } @ SuperClass;
    },

    initSystems = -> {
        this.world->addSystem(ECS.Systems.PanControllerSystem.create());
        this.world->addSystem(Systems.PlatformSystem.create());
        this.world->addSystem(Systems.PatrollingSystem.create("Player"));
        this.world->addSystem(ECS.Systems.RigidbodySystem.create());
        this.world->addSystem(ECS.Systems.FrameAnimationSystem.create());
        this.world->addSystem(ECS.Systems.CameraSystem.create());

        this.world->addSystem(ECS.Systems.CollisionSystem.create());
        this.world->addSystem(Systems.PlayerSystem.create());
        this.world->addSystem(ECS.Systems.TilemapCollisionSystem.create());
        this.world->addSystem(ECS.Systems.TilemapRenderSystem.create(), ECS.World.Phase.Draw);
        this.world->addSystem(ECS.Systems.SpriteRenderSystem.create(), ECS.World.Phase.Draw);
        this.world->addSystem(ECS.Systems.LabelRenderSystem.create(), ECS.World.Phase.Draw);
        this.world->addSystem(ECS.Systems.LabelRenderSystem.create(), ECS.World.Phase.DrawUI);
    },

    init = -> {
        let gameOverText = this.world->getTaggedEntities("Game Over Label")[0];
        
        let size = gameOverText.Label.font->getTextSize(gameOverText.Label.text);
        gameOverText.Transform.position.x = (Kolanut.getScreenSize().x * 0.5) - (size.x * 0.5);
        gameOverText.Transform.position.y = (Kolanut.getScreenSize().y * 0.5) - (size.y * 0.5);
        gameOverText.Label.enabled = false;

        this.player = this.world->getTaggedEntities("Player")[0];
    },

    onKeyPressed = |key, pressed| -> {
        this.input[key] = pressed; 
    },

    update = |dt| -> {
        this.player.PlatformController.controls.left = this.input.left;
        this.player.PlatformController.controls.right = this.input.right;
        this.player.PlatformController.controls.jump = this.input.up;
        this.player.PlatformController.controls.dash = this.input.space;
    }
};

return Game;