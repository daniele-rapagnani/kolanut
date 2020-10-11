let Component = import("./Component");

let Tilemap = {
    create = |tilemap| => {
        return { 
            tilemap = tilemap
        } @ Tilemap;
    },
    
    getType = -> {
        return "Tilemap";
    }
} @ Component.create();

return Tilemap;