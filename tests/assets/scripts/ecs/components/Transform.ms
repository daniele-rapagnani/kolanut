let Component = import("./Component");

let Transform = {
    create = => {
        return { 
            position = Vector.create(0.0, 0.0), 
            angle = 0.0, 
            scale = Vector.create(1.0, 1.0) 
        } @ Transform;
    },

    getType = => {
        return "Transform";
    }
} @ Component.create();

return Transform;