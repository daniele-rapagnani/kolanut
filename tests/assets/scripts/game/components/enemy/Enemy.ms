let Component = import("ecs/components/Component");

let Enemy = {
    create = => {
        return { } @ Enemy;
    },
    
    getType = -> {
        return "Enemy";
    }
} @ Component.create();

return Enemy;