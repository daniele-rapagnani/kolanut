let Component = import("../../Component");

let Player = {
    create = => {
        return { } @ Player;
    },
    
    getType = -> {
        return "Player";
    }
} @ Component.create();

return Player;