let System = import("ecs/systems/System");

let PatrollingSystem = {
    create = => {
        return { } @ PatrollingSystem;
    },

    update = |dt, entities| -> {
        let self = this;
        
        this.updateWithComponents(
            entities,
            ["Transform", "PatrolController", "TileCollider", "PlatformController"],
            |components| => {
                let pc = components.PatrolController;
                let ptc = components.PlatformController;
                let tr = components.Transform;
                let tc = components.TileCollider;

                func updateTarget |newTarget| => {
                    pc.curTarget = newTarget;

                    ptc.controls.right = false;
                    ptc.controls.left = false;

                    if (tr.position.x < newTarget) {
                        ptc.controls.right = true;
                    } else {
                        ptc.controls.left = true;
                    }
                }

                if (pc.curTarget == null && tc.colliding.bottom) {
                    updateTarget(tr.position.x - pc.distance);
                }

                if (!tc.colliding.bottom) {
                    if (!tc.colliding.bottomRight) {
                        updateTarget(tr.position.x - pc.distance);
                    } else if (!tc.colliding.bottomLeft) {
                        updateTarget(tr.position.x + pc.distance);
                    } else {
                        ptc.controls.left = false;
                        ptc.controls.right = false;
                    }
                }

                let targets = self.world->getTaggedEntities(pc.targetTag);
                
                if (#targets > 0) {
                    let dist = tr.position - targets[0].Transform.position;
                    
                    if (#dist < pc.attackDist) {
                        if (
                            (tr.position.x > targets[0].Transform.position.x && ptc.controls.left)
                            || (tr.position.x < targets[0].Transform.position.x && ptc.controls.right)
                        ) {
                            ptc.controls.dash = true;
                        }
                    }
                }
            }
        );
    }
} @ System.create();

return PatrollingSystem;