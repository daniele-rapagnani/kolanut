let System = import("./System");
let World = import("../World");

let LabelRenderSystem = {
    create = => {
        return { } @ LabelRenderSystem;
    },

    update = |dt, entities, phase| -> {
        this.updateWithComponents(
            entities, 
            ["Transform", "Label"],
            |components| => {
                let lb = components.Label;

                if (lb.inWorldView && phase != World.Phase.Draw) {
                    return;
                }

                let tr = components.Transform;
                lb.font->draw(lb.text, tr.position, lb.size, lb.color);
            }
        );
    }
} @ System.create("LabelRenderSystem");

return LabelRenderSystem;