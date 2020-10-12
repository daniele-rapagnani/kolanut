let System = {
    create = |name| => {
        return { world = null, name = name, enabled = true } @ System;
    },

    getName = -> {
        return this.name;
    },

    updateWithComponents = |entities, components, update| => {
        if (!types.isArray(components)) {
            components = [components];
        }

        for (let e in entities) {
            let foundComps = {};

            for (let c in components) {
                let fc = e->getComponent(c);

                if (fc == null || !fc.enabled) {
                    continue;
                }

                foundComps[fc->getType()] = fc;
            }

            if (#foundComps != #components)
            {
                continue;
            }

            update(foundComps, e);
        }
    }
};

return System;