let Component = import("./Component");

let Transform = {
    create = => {
        return { 
            position = Vector.create(0.0, 0.0), 
            angle = 0.0, 
            scale = Vector.create(1.0, 1.0),
            origin = Vector.create(0.0, 0.0),
            parent = null,
            dirty = true
        } @ Transform;
    },

    [object.symbols.setPropertyOperator] = |prop| -> {
        if (prop != "dirty") {
            this.dirty = true;
        }

        return false;
    },

    getType = -> {
        return "Transform";
    }
} @ Component.create();

return Transform;