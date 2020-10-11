let Component = import("ecs/components/Component");

let Player = {
    create = => {
        return {
            onDeath = null
        } @ Player;
    },
    
    getType = -> {
        return "Player";
    }
} @ Component.create();

return Player;