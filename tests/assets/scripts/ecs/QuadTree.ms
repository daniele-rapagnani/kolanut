let QuadTree = {
    MAX_OCCUPANCY = 1,
    MIN_SIZE = 16,

    create = |bb, parent| => {
        let inst = {
            entries = [],
            quadrants = [],
            parent = parent,
            bb = bb ?? BoundingBox.create()
        } @ QuadTree;

        array.resize(inst.quadrants, 4);

        return inst;
    },

    isLeaf = -> {
        return this.quadrants[0] == null;
    },

    clear = -> {
        this.entries = [];

        if (!this->isLeaf()) {
            this.quadrants[0].parent = null;
            this.quadrants[1].parent = null;
            this.quadrants[2].parent = null;
            this.quadrants[3].parent = null;
        }

        this.quadrants[0] = null;
        this.quadrants[1] = null;
        this.quadrants[2] = null;
        this.quadrants[3] = null;
    },

    isEmpty = -> {
        return #this.entries == 0;
    },

    split = func quadTreeSplit -> {
        let subBB = BoundingBox.createFromPos(
            this.bb->getOrigin(),
            this.bb->getSize() * 0.5
        );

        let s = subBB->getSize();

        this.quadrants[0] = QuadTree.create(subBB, this);
        this.quadrants[1] = QuadTree.create(subBB + Vector2.create(s.x, 0), this);
        this.quadrants[2] = QuadTree.create(subBB + Vector2.create(0, s.y), this);
        this.quadrants[3] = QuadTree.create(subBB + s, this);

        for (let entry in this.entries) {
            this.quadrants[0]->insert(entry.entity, entry.bb);
            this.quadrants[1]->insert(entry.entity, entry.bb);
            this.quadrants[2]->insert(entry.entity, entry.bb);
            this.quadrants[3]->insert(entry.entity, entry.bb);
        }

        array.resize(this.entries, 0);
    },

    insert = |e, bb| -> {
        let contains = this.bb->contains(bb);

        if (!contains && !this.bb->overlaps(bb)) {
            return false;
        }

        if (this->isLeaf()) {
            if (
                #this.entries < QuadTree.MAX_OCCUPANCY
                || math.round(this.bb->getSize().x) <= QuadTree.MIN_SIZE
            ) 
            {
                this.entries[] = { entity = e, bb = bb };
                return true;
            }

            this->split();
        }

        if (this.quadrants[0]->insert(e, bb) && contains) {
            return true;
        }

        if (this.quadrants[1]->insert(e, bb) && contains) {
            return true;
        }

        if (this.quadrants[2]->insert(e, bb) && contains) {
            return true;
        }

        if (this.quadrants[3]->insert(e, bb) && contains) {
            return true;
        }

        return true;
    },

    query = |bb, res, exclude| -> {
        if (!this.bb->overlaps(bb)) {
            return;
        }

        if (#this.entries > 0) {
            for (let entry in this.entries) {
                if (entry.entity == exclude) {
                    continue;
                }

                res[] = entry;
            }
        }

        if (this->isLeaf()) {
            return;
        }

        this.quadrants[0]->query(bb, res, exclude);
        this.quadrants[1]->query(bb, res, exclude);
        this.quadrants[2]->query(bb, res, exclude);
        this.quadrants[3]->query(bb, res, exclude);
    },

    visit = |f| -> {
        f(this);

        if (this->isLeaf()) {
            return;
        }

        this.quadrants[0]->visit(f);
        this.quadrants[1]->visit(f);
        this.quadrants[2]->visit(f);
        this.quadrants[3]->visit(f);
    }
};

return QuadTree;