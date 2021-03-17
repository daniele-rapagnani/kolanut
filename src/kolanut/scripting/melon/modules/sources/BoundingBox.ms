let BoundingBox = {
    create = func bbCreate |left, top, right, bottom| => {
        return { 
            left = number.fromNumber(left ?? 0),
            top = number.fromNumber(top ?? 0),
            right = number.fromNumber(right ?? 0),
            bottom = number.fromNumber(bottom ?? 0)
        } @ BoundingBox;
    },
    
    createFromPos = func bbCreateFromPos |position, size| => {
        return BoundingBox.create(
            position.x,
            position.y,
            position.x + size.x,
            position.y + size.y
        );
    },

    createFromRect = func bbCreateFromRect |rect| => {
        return BoundingBox.createFromPos(rect.origin, rect.size);
    },

    getSize = -> {
        return Vector2.create(
            this.right - this.left,
            this.bottom - this.top
        );
    },

    getOrigin = -> {
        return Vector2.create(this.left, this.top);
    },

    overlaps = |bb| -> {
        let overlapH = (this.left < bb.right && this.right > bb.left);
        let overlapV = (this.bottom > bb.top && this.top < bb.bottom);

        return overlapH && overlapV;
    },

    contains = |bb| -> {
        return bb.left >= this.left
            && bb.right <= this.right
            && bb.top >= this.top
            && bb.bottom <= this.bottom
        ;
    },

    [object.symbols.sumOperator] = |other| -> {
        if (types.isObject(other)) {
            if ((@other).create == BoundingBox.create) {
                let res = BoundingBox.create(
                    this.left,
                    this.top,
                    this.right,
                    this.bottom
                );

                if (res.left < other.left) {
                    this.left = other.left;
                }

                if (res.right < other.right) {
                    this.right = other.right;
                }

                if (res.top < other.top) {
                    this.top = other.top;
                }

                if (res.bottom < other.bottom) {
                    this.bottom = other.bottom;
                }

                return res;
            } else if ((@other).create == Vector2.create) {
                return BoundingBox.create(
                    this.left + other.x,
                    this.top + other.y,
                    this.right + other.x,
                    this.bottom + other.y
                );
            }
        }

        return false;
    },

    [object.symbols.mulOperator] = func bbMultiply |other| -> {
        if (types.isObject(other) && (@other).create == Kolanut.Transform3D.create) {
            let res = BoundingBox.create();

            let min = other * Vector4.create(this.left, this.top, 0.0, 1.0);
            let max = other * Vector4.create(this.right, this.bottom, 0.0, 1.0);

            if (min.x < max.x) {
                res.left = min.x;
                res.right = max.x;
            } else {
                res.left = max.x;
                res.right = min.x;
            }

            if (min.y < max.y) {
                res.top = min.y;
                res.bottom = max.y;
            } else {
                res.top = max.y;
                res.bottom = min.y;
            }

            return res;
        }

        return BoundingBox.createFromPos(
            this->getOrigin(),
            this->getSize() * other
        );
    },
    [object.symbols.sizeOperator] = -> {
        return this->getSize().x * this->getSize().y;
    },
    [object.symbols.compareOperator] = |other| -> {
        if (!types.isObject(other))
        {
            return 1;
        }
        
        if (
            this.top == other.top
            && this.left == other.left
            && this.right == other.right
            && this.bottom == other.bottom
        )
        {
            return 0;
        }

        return math.round(#this - #other);
    }
};

return BoundingBox;