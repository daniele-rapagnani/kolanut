let System = import("ecs/systems/System");
let ECS = import("ecs");
let Events = import("../events");

let PlayerSystem = {
    create = => {
        Events.OnPlayerDeath->subscribeSystem("PlayerSystem");
        Events.OnPlayerFall->subscribeSystem("PlayerSystem");
        Events.OnPlayerHit->subscribeSystem("PlayerSystem");
        ECS.Events.Collision->subscribeSystem("PlayerSystem");
        
        return { } @ PlayerSystem;
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
                Events.OnPlayerHit->trigger(this.world);
            }
        }
    },

    onPlayerHit = -> {
        let player = this.world->getTaggedEntities("Player")[0];

        if (player.Player.invincible || player.Player.dead) {
            return;
        }

        player.Player.energy = player.Player.energy - 1;

        if (player.Player.sounds.hit != null) {
            player.Player.sounds.hit->play();
        }

        if (player.Player.energy <= 0) {
            Events.OnPlayerDeath->trigger(this.world);
            return;
        }

        player.Player.invincible = true;
        player.PlatformController.overrideAnimation = true;
        player.FrameAnimation->playAnimation("hit", false);

        player.FrameAnimation.finishCallback = => {
            player.Player.invincible = false;
            player.PlatformController.overrideAnimation = false;
            player.FrameAnimation.finishCallback = null;
        };
    },

    killPlayer = |disableComps, animation| -> {
        let player = this.world->getTaggedEntities("Player")[0];

        if (player == null || player.Player.dead) {
            return;
        }

        player.Player.dead = true;

        if (player.Player.sounds.death != null) {
            player.Player.sounds.death->play();
        }

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
        let camera = this.world->getTaggedEntities("Camera")[0].Camera;

        this.updateWithComponents(
            entities,
            ["Transform", "Player"],
            |components| => {
                if (
                    camera.bounds.bottom != null 
                    && components.Transform.position.y > camera.bounds.bottom
                ) {
                    Events.OnPlayerFall->trigger(self.world);
                }
            }
        );
    }
} @ System.create("PlayerSystem");

return PlayerSystem;