let Component = import("ecs/components/Component");

let PlatformController = {
    create = => {
        return {
            speed = 100,
            jumpSpeed = 200,
            dashingTime = 0.20,
            dashingHold = 0.0,
            dashingSpeedMult = 3.0,
            dashingRecharge = 0.3,
            curDashingTime = null,
            dashSpeed = null,
            overrideAnimation = false,
            animations = {
                run = "run",
                idle = "idle",
                dash = "dash"
            },
            controls = {
                left = false,
                right = false,
                jump = false,
                dash = false
            }
        } @ PlatformController;
    },
    
    getType = -> {
        return "PlatformController";
    }
} @ Component.create();

return PlatformController;