let ECS = import("ecs");

let OnPlayerHit = {
} @ ECS.Events.Event.create("onPlayerHit");

return OnPlayerHit;