let ECS = import("ecs");

let OnPlayerDeath = {
} @ ECS.Events.Event.create("onPlayerDeath");

return OnPlayerDeath;