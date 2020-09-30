let Component = import("../../Component");

let PatrolController = {
    create = => {
        return {
            distance = 70.0,
            curTarget = null,
            startedAt = null,
            attackDist = 30.0,
            targetTag = "Player"
        } @ PatrolController;
    },
    
    getType = -> {
        return "PatrolController";
    }
} @ Component.create();

return PatrolController;