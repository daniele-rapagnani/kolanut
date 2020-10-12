let System = import("./System");

let TilemapCollisionSystem = {
    create = |tilemap| => {
        return { tilemap = tilemap } @ TilemapCollisionSystem;
    },

    update = |dt, entities| -> {
        let self = this;

        if (this.tilemap == null) {
            let tilemapEntity = this.world->getTaggedEntities("Tilemap");

            if (#tilemapEntity > 0 && tilemapEntity[0].Tilemap != null) {
                this.tilemap = tilemapEntity[0].Tilemap.tilemap;
            }
        }

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

                    let bb = null;

                    if (fa != null) {
                        bb = fa->getBB(tr);
                    } else {
                        bb = sr->getBB(tr);
                    }

                    let w = bb.right - bb.left;
                    let h = bb.bottom - bb.top;
                    let pos = Vector.create(bb.left, bb.top);

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

                    let bottomLeftTile = self.tilemap->getTileAt(
                        tc.layer,
                        pos + Vector.create(-1.0, h + 1.0)
                    );

                    let bottomRightTile = self.tilemap->getTileAt(
                        tc.layer,
                        pos + Vector.create(w + 1.0, h + 1.0)
                    );

                    let rightTile = self.tilemap->getTileAt(
                        tc.layer, 
                        pos + Vector.create(w + 1.0, h * 0.5)
                    );

                    if (rightTile != null) {
                        tr.position.x = rightTile.position.x - (w  - tr.origin.x);
                        
                        if (rb.velocity.x > 0.0) {
                            rb.velocity.x = 0.0;
                        }
                    }

                    let leftTile = self.tilemap->getTileAt(
                        tc.layer, 
                        pos + Vector.create(-1.0, h * 0.5)
                    );

                    if (leftTile != null) {
                        tr.position.x = leftTile.position.x + leftTile.rect.w + tr.origin.x;
                        
                        if (rb.velocity.x < 0.0) {
                            rb.velocity.x = 0.0;
                        }
                    }

                    tc.colliding = {
                        bottom = bottomTile != null,
                        left = leftTile != null,
                        right = rightTile != null,
                        bottomLeft = bottomLeftTile != null,
                        bottomRight = bottomRightTile != null
                    };
                }
            );
        }
    }
} @ System.create("TilemapCollisionSystem");

return TilemapCollisionSystem;