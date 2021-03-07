let Component = import("ecs/components/Component");

let Player = {
    create = => {
        return {
            onDeath = null,
            dead = false,
            invincible = false,
            energy = 3,
            sounds = {
                death = null,
                hit = null
            }
        } @ Player;
    },
    
    getType = -> {
        return "Player";
    }
} @ Component.create();

return Player;