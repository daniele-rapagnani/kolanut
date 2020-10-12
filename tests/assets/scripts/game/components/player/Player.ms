let Component = import("ecs/components/Component");

let Player = {
    create = => {
        return {
            onDeath = null,
            dead = false
        } @ Player;
    },
    
    getType = -> {
        return "Player";
    }
} @ Component.create();

return Player;