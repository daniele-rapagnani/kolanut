let Component = import("../Component");

let PanController = {
    create = => {
        return { 
            left = false,
            right = false,
            up = false,
            down = false,
            speed = 500
        } @ PanController;
    },

    getType = -> {
        return "PanController";
    }
} @ Component.create();

return PanController;