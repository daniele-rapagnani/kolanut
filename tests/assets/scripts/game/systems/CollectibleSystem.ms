let System = import("ecs/systems/System");
let ECS = import("ecs");
let Events = import("../events");

let CollectibleSystem = {
    create = => {
        ECS.Events.Collision->subscribeSystem("CollectibleSystem");
        return { } @ CollectibleSystem;
    },

    onCollision = |a, b| -> {
        let player = null;
        let other = null;

        if (a.Player != null) {
            player = a;
            other = b;
        } else if (b.Player != null) {
            player = b;
            other = a;
        }

        if (player == null) {
            return;
        }

        if (other.Collectible != null) {
            if (other.Collectible.sounds.collected != null) {
                other.Collectible.sounds.collected->play();
            }

            other.world->removeEntity(other);
        }
    },

    update = |dt, entities| -> { }
} @ System.create("CollectibleSystem");

return CollectibleSystem;