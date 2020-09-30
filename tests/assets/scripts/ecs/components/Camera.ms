let Component = import("./Component");

let Camera = {
    create = => {
        return {
            followTarget = null
        } @ Camera;
    },

    getType = -> {
        return "Camera";
    }
} @ Component.create();

return Camera;