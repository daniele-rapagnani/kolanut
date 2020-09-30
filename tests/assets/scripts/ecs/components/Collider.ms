let Component = import("./Component");

let Collider = {
    Types = {
        RendererBB = "RendererBB"
    },

    create = => {
        return { type = Collider.Types.RendererBB, collidedWith = null } @ Collider;
    },

    getType = -> {
        return "Collider";
    }
} @ Component.create();

return Collider;