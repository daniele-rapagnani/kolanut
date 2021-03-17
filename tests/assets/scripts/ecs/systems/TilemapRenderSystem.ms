let System = import("./System");

let TilemapRenderSystem = {
    create = => {
        return { } @ TilemapRenderSystem;
    },

    update = |dt, entities| -> {
        // Kolanut.Tracy.beginZone("Tilemap draw");
        // this.updateWithComponents(
        //     entities, 
        //     ["Tilemap"],
        //     |components| => {
        //         components.Tilemap.tilemap->draw();
        //     }
        // );
        // Kolanut.Tracy.endZone();
    }
} @ System.create("TilemapRenderSystem");

return TilemapRenderSystem;