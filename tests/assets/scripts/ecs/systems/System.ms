let System = {
    create = => {
        return { } @ System;
    },

    updateWithComponents = |entities, components, update| => {
        if (!types.isArray(components)) {
            components = [components];
        }

        for (let e in entities) {
            let foundComps = {};

            for (let c in components) {
                let fc = e->getComponent(c);

                if (fc == null) {
                    continue;
                }

                foundComps[fc.getType()] = fc;
            }

            update(foundComps);
        }
    }
};

return System;