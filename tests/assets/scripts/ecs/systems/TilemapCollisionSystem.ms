let System = import("./System");

let TilemapCollisionSystem = {
    create = |tilemap| => {
        return { tilemap = tilemap } @ TilemapCollisionSystem;
    },

    getTileRange = |layer, bb, padding, lock, vertical| -> {
        let pos = bb->getOrigin();
        let size = bb->getSize();
        let dist = null;
        
        if (vertical) {
            dist = size.y - padding * 2.0;
        } else {
            dist = size.x - padding * 2.0;
        }

        let inc = dist / 3.0;

        let point = null;
        
        if (vertical) {
            point = Vector2.create(pos.x + lock * size.x, pos.y + padding);
        } else {
            point = Vector2.create(pos.x + padding, pos.y + lock * size.y);
        }

        for (let i in 0..4) {
            let tile = this.tilemap->getTileAt(layer, point);

            if (tile != null) {
                return tile;
            }

            if (vertical) {
                point.y = point.y + inc;
            } else { 
                point.x = point.x + inc;
            }
        }

        return null;
    },

    update = func tilemapSystemUpdate |dt, entities| -> {
        Kolanut.Tracy.beginZone("Tilemap collisions update");
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

                    let size = null;

                    if (fa != null) {
                        size = fa->getSize();
                    } else {
                        size = sr->getSize();
                    }

                    let bb = tr->getBB(size);

                    let w = bb->getSize().x;
                    let h = bb->getSize().y;
                    let pos = Vector2.create(bb.left, bb.top);

                    let topTile = self->getTileRange(tc.layer, bb, tc.padding.h, 0.0, false);

                    if (topTile != null) {
                        let point = (topTile.position.y + topTile.rect.size.y);

                        if (bb.top < point) {
                            tr.position.y = tr.position.y + (point - bb.top);
                        }

                        if (rb.velocity.y < 0.0) {
                            rb.velocity.y = 0;
                        }
                    }

                    let bottomTile = self->getTileRange(tc.layer, bb, tc.padding.h, 1.0, false);

                    if (bottomTile != null) {
                        let point = bottomTile.position.y;

                        if (bb.bottom > point) {
                            tr.position.y = tr.position.y - (bb.bottom - point);
                        }

                        if (rb.velocity.y > 0.0) {
                            rb.velocity.y = 0;
                        }
                    }

                    let bottomLeftTile = self.tilemap->getTileAt(
                        tc.layer,
                        pos + Vector2.create(-1.0, h + 1.0)
                    );

                    let bottomRightTile = self.tilemap->getTileAt(
                        tc.layer,
                        pos + Vector2.create(w + 1.0, h + 1.0)
                    );

                    let rightTile = self->getTileRange(tc.layer, bb, tc.padding.v, 1.0, true);

                    if (rightTile != null) {
                        let point = rightTile.position.x;
                        
                        if (bb.right > point) {
                            tr.position.x = tr.position.x - (bb.right - point);
                        }
                        
                        if (rb.velocity.x > 0.0) {
                            rb.velocity.x = 0.0;
                        }
                    }

                    let leftTile = self->getTileRange(tc.layer, bb, tc.padding.v, 0, true);

                    if (leftTile != null) {
                        let point = leftTile.position.x + leftTile.rect.size.x;

                        if (bb.left < point) {
                            tr.position.x = tr.position.x + (point - bb.left);
                        }

                        if (rb.velocity.x < 0.0) {
                            rb.velocity.x = 0.0;
                        }
                    }

                    tc.colliding = {
                        top = topTile != null,
                        bottom = bottomTile != null,
                        left = leftTile != null,
                        right = rightTile != null,
                        bottomLeft = bottomLeftTile != null,
                        bottomRight = bottomRightTile != null
                    };
                }
            );
        }
        
        Kolanut.Tracy.endZone();
    }
} @ System.create("TilemapCollisionSystem");

return TilemapCollisionSystem;