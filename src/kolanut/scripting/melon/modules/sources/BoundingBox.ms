/***
 * @module
 * 
 * This class represents a 2D axis aligned bounding box.
 */

let BoundingBox = {

    /***
     * Creates a new BoundingBox object
     * 
     * @arg left The min x of this AABB
     * @arg top The min y of this AABB
     * @arg right The max x of this AABB
     * @arg bottom The max y of this AABB
     *
     * @returns A new BoundingBox object
     */

    create = func bbCreate |left, top, right, bottom| => {
        return { 
            left = number.fromNumber(left ?? 0),
            top = number.fromNumber(top ?? 0),
            right = number.fromNumber(right ?? 0),
            bottom = number.fromNumber(bottom ?? 0)
        } @ BoundingBox;
    },

    /***
     * Creates a new BoundingBox given a position and a size
     * 
     * @arg position A `Vector2` with the AABB position
     * @arg size A `Vector2` holding the AABB's size
     *
     * @returns A new BoundingBox object
     */
    
    createFromPos = func bbCreateFromPos |position, size| => {
        return BoundingBox.create(
            position.x,
            position.y,
            position.x + size.x,
            position.y + size.y
        );
    },

    /***
     * Creates a new BoundingBox from a given `Rect`.
     * 
     * @arg rect A `Rect` holding the coordinates of this AABB.
     *
     * @returns A new BoundingBox object
     */

    createFromRect = func bbCreateFromRect |rect| => {
        return BoundingBox.createFromPos(rect.origin, rect.size);
    },

    /***
     * Gets the size of this AABB.
     *
     * @returns A `Vector2` holding the size of this AABB
     */

    getSize = -> {
        return Vector2.create(
            this.right - this.left,
            this.bottom - this.top
        );
    },

    /***
     * Gets the origin of this AABB.
     *
     * @returns A `Vector2` holding the origin of this AABB
     */

    getOrigin = -> {
        return Vector2.create(this.left, this.top);
    },

    /***
     * Checks whether this AABB overlaps another.
     *
     * @arg bb The other AABB that may overlap this.
     *
     * @returns `true` if they overlap, `false` otherwise
     */

    overlaps = |bb| -> {
        let overlapH = (this.left < bb.right && this.right > bb.left);
        let overlapV = (this.bottom > bb.top && this.top < bb.bottom);

        return overlapH && overlapV;
    },

    /***
     * Checks whether this AABB contains another.
     *
     * @arg bb The AABB that may be contained by this one.
     *
     * @returns `true` if this ABBB contains *bb*, `false` otherwise
     */

    contains = |bb| -> {
        return bb.left >= this.left
            && bb.right <= this.right
            && bb.top >= this.top
            && bb.bottom <= this.bottom
        ;
    },

    /***
     * If the rhs object is another BoundingBox the result 
     * will be the union of the two AABBs.
     * If the rhs object is a Vector2 the result will be 
     * a new AABB translated by that vector.
     */

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

    /***
     * If the rhs object is a `Transform3D` then the result will be
     * the resulting transformed AABB.
     * If the rhs value it will be multiplied to the size of the lhs AABB
     * and a new AABB will be returned.
     */

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

    /***
     * Returns the area of this AABB
     */
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