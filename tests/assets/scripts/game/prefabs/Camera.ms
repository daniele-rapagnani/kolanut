let ECS = import("ecs");

let Camera = {
    create = |world| => {
        let camera = ECS.Entity.create();
        camera->addComponent(ECS.Components.Transform.create());
        camera->addComponent(ECS.Components.Camera.create());
        camera->addComponent(ECS.Components.PanController.create());

        camera.Transform.scale.x = 2.0;
        camera.Transform.origin = Kolanut.getScreenSize() * 0.5;
        camera.Camera.followTag = "Player";

        camera->setTag("Camera");

        world->addEntity(camera);

        return camera;
    }
};

return Camera;