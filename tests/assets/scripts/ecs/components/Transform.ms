let Component = import("./Component");

let Transform = {
    create = => {
        return { 
            position = Vector2.create(0.0, 0.0), 
            angle = 0.0, 
            scale = Vector2.create(1.0, 1.0),
            origin = Vector2.create(0.0, 0.0),
        } @ Transform;
    },
    
    getType = -> {
        return "Transform";
    }
} @ Component.create();

return Transform;