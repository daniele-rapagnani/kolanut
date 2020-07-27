let Component = import("../../Component");

let PlatformController = {
    create = => {
        return {
            speed = 100,
            jumpSpeed = 200,
            animations = {
                run = "run",
                idle = "idle"
            },
            controls = {
                left = false,
                right = false,
                jump = false
            }
        } @ PlatformController;
    },
    
    getType = -> {
        return "PlatformController";
    }
} @ Component.create();

return PlatformController;