let Component = import("./Component");

let Rigidbody = {
    create = => {
        return {
            mass = 1.0,
            acceleration = Vector.create(0.0, 0.0),
            velocity = Vector.create(0.0, 0.0),
            angularVelocity = 0.0,
            damping = 0.8,
            gravity = Vector.create(0, 500.0),
            maxVelocity = null
        } @ Rigidbody;
    },

    setForce = |force| -> {
        this.acceleration = force / this.mass;
    },

    getType = -> {
        return "Rigidbody";
    }
} @ Component.create();

return Rigidbody;