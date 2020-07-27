let System = import("./System");

let TilemapCollisionSystem = {
    TileColliderCollidingSymbol = $"colliding"$,

    create = |tilemap| => {
        return { tilemap = tilemap } @ TilemapCollisionSystem;
    },

    update = |dt, entities| -> {
        let self = this;

        if (this.tilemap != null) {
            this.updateWithComponents(
                entities, 
                ["Transform", "Rigidbody", "TileCollider"],
                |components, entity| => {
                    let tr = components.Transform;
                    let rb = components.Rigidbody;
                    let tc = components.TileCollider;
                    let sr = entity.SpriteRenderer;
                    let fa = entity.FrameAnimation;

                    let pos = tr.position - tr.origin;
                    let w = 0;
                    let h = 0;

                    if (fa != null && fa->getCurAnimation() != null) {
                        w = fa->getCurAnimation().frameSize.w;
                        h = fa->getCurAnimation().frameSize.h;
                    } else {
                        w = sr->getSize().w;
                        h = sr->getSize().h;
                    }

                    let bottomTile = self.tilemap->getTileAt(
                        tc.layer, 
                        pos + Vector.create(w * 0.5, h + 1.0)
                    );

                    if (bottomTile != null) {
                        tr.position.y = bottomTile.position.y - (h - tr.origin.y);

                        if (rb.velocity.y > 0.0) {
                            rb.velocity.y = 0;
                        }
                    }
                    
                    let rightTile = self.tilemap->getTileAt(
                        tc.layer, 
                        pos + Vector.create(w + 1.0, h * 0.5)
                    );

                    if (rightTile != null) {
                        tr.position.x = rightTile.position.x - (w  - tr.origin.x);
                        
                        if (rb.velocity.x > 0.0) {
                            rb.velocity.x = 0;
                        }
                    }

                    let leftTile = self.tilemap->getTileAt(
                        tc.layer, 
                        pos + Vector.create(-1.0, h * 0.5)
                    );

                    if (leftTile != null) {
                        tr.position.x = leftTile.position.x + leftTile.rect.w + tr.origin.x;
                        
                        if (rb.velocity.x < 0.0) {
                            rb.velocity.x = 0;
                        }
                    }

                    tc.colliding = {
                        bottom = bottomTile != null,
                        left = leftTile != null,
                        right = rightTile != null
                    };
                }
            );
        }
    }
} @ System.create();

return TilemapCollisionSystem;