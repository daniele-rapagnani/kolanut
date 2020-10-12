let Utils = import("utils");

let Event = { 
    create = |name| => {
        return { name = name, systems = [], systemsMap = {} } @ Event;
    },

    subscribeSystem = |system| -> {
        if (this.systemsMap[system] == null) {
            this.systems[] = system;
            this.systemsMap[system] = true;
        }
    },

    unsubscribeSystem = |system| -> {
        if (this.systemsMap[system] != null) {
            this.systemsMap[system] = null;
            Utils.Array.delete(this.systems, system);
        }
    },

    trigger = |world, ...args| -> {
        for (let s in this.systems) {
            let sysObj = world->getSystem(s);

            if (sysObj == null) {
                continue;
            }

            if (sysObj[this.name] == null) {
                continue;
            }

            function.call(sysObj[this.name], [sysObj] .. args);
        }
    }
};

return Event;