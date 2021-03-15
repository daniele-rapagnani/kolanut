let System = import("./System");

let RigidbodySystem = {
    create = => {
        return { } @ RigidbodySystem;
    },

    update = |dt, entities| -> {
        Kolanut.Tracy.beginZone("Rigidbody update");
        this.updateWithComponents(
            entities,
            ["Transform", "Rigidbody"],
            |components| => {
                let rb = components.Rigidbody;
                let tr = components.Transform;

                rb.velocity = rb.velocity + (rb.gravity * dt);
                rb.velocity = rb.velocity + (rb.acceleration * dt);

                if (rb.maxVelocity != null) {
                    if (rb.velocity.x > rb.maxVelocity.x) {
                        rb.velocity.x = rb.maxVelocity.x;
                    }

                    if (rb.velocity.y > rb.maxVelocity.y) {
                        rb.velocity.y = rb.maxVelocity.y;
                    }
                }

                tr.position = tr.position + (rb.velocity * dt);
                tr.angle = tr.angle + (rb.angularVelocity * dt);
            }
        );
        Kolanut.Tracy.endZone();
    }
} @ System.create("RigidbodySystem");

return RigidbodySystem;