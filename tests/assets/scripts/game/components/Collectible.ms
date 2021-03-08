let Component = import("ecs/components/Component");

let Collectible = {
    create = => {
        return {
            sounds = {
                collected = null
            }
        } @ Collectible;
    },
    
    getType = -> {
        return "Collectible";
    }
} @ Component.create();

return Collectible;