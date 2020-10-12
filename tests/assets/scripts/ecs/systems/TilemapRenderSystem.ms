let System = import("./System");

let TilemapRenderSystem = {
    create = => {
        return { } @ TilemapRenderSystem;
    },

    update = |dt, entities| -> {
        this.updateWithComponents(
            entities, 
            ["Tilemap"],
            |components| => {
                components.Tilemap.tilemap->draw();
            }
        );
    }
} @ System.create("TilemapRenderSystem");

return TilemapRenderSystem;