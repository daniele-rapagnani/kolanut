let Component = import("./Component");

let Camera = {
    create = => {
        return {
            followTag = null,
            bounds = {
                left = null,
                top = null,
                right = null,
                bottom = null
            }
        } @ Camera;
    },

    getBB = |tr| -> {
        let ss = Kolanut.getDesignResolution();
        let zoom = 1.0 / tr.scale.x;

        let bb = {
            top = tr.position.y - (tr.origin.y * zoom),
            left = tr.position.x - (tr.origin.x * zoom),
        };

        bb.right = bb.left + (ss.x * zoom);
        bb.bottom = bb.top + (ss.y * zoom);

        return bb;
    },

    getType = -> {
        return "Camera";
    }
} @ Component.create();

return Camera;