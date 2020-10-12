let System = import("ecs/systems/System");
let ECS = import("ecs");
let Events = import("../events");

let PlayerSystem = {
    create = => {
        Events.OnPlayerDeath->subscribeSystem("PlayerSystem");
        Events.OnPlayerFall->subscribeSystem("PlayerSystem");
        ECS.Events.Collision->subscribeSystem("PlayerSystem");
        
        return { 
            fallDoomY = 1121.0
        } @ PlayerSystem;
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

        if (other.Enemy != null) {
            if (other.FrameAnimation.curAnimation == "attack") {
                Events.OnPlayerDeath->trigger(this.world);
            }
        }
    },

    killPlayer = |disableComps, animation| -> {
        let player = this.world->getTaggedEntities("Player")[0];

        if (player.Player.dead) {
            return;
        }

        player.Player.dead = true;

        for (let c in disableComps) {
            player->getComponent(c).enabled = false;
        }

        let camera = this.world->getTaggedEntities("Camera")[0];
        camera.Camera.followTag = null;

        if (animation != null) {
            let self = this;

            player.FrameAnimation.finishCallback = |e| => {
                self.world->removeEntity(e);
                
                if (player.Player.onDeath != null) {
                    player.Player.onDeath();
                }
            };

            player.FrameAnimation->playAnimation(animation, false);
        } else {
            if (player.Player.onDeath != null) {
                player.Player.onDeath();
            }
        }
    },

    onPlayerFall = -> {
        this->killPlayer(["Player", "PlatformController", "Collider"]);
    },

    onPlayerDeath = -> {
        this->killPlayer(["Player", "PlatformController", "Collider", "Rigidbody"], "die");    
    },

    update = |dt, entities| -> {
        let self = this;

        this.updateWithComponents(
            entities,
            ["Transform", "Player"],
            |components| => {
                if (components.Transform.position.y > self.fallDoomY) {
                    Events.OnPlayerFall->trigger(self.world);
                }
            }
        );
    }
} @ System.create("PlayerSystem");

return PlayerSystem;