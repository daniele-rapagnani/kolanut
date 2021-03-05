let ECS = import("ecs");
let Assets = import("../Assets");

let GameOverLabel = {
    create = |world| => {
        let gameOverText = ECS.Entity.create();
        gameOverText->addComponent(ECS.Components.Transform.create());
        gameOverText->addComponent(
            ECS.Components.Label.create(Assets.Fonts.Pixel, "Game Over")
        );

        gameOverText->setTag("Game Over Label");
        world->addEntity(gameOverText);

        return gameOverText;
    }
};

return GameOverLabel;