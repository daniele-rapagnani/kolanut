let ECS = import("ecs");

let OnPlayerFall = {
} @ ECS.Events.Event.create("onPlayerFall");

return OnPlayerFall;