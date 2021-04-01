/***
 * @module
 *
 * This class represents rectangles in 2D space
 */

let Rect = {
    /***
     * Creates a rectangle given an origin and a size
     *
     * @arg origin A `Vector2` pointing to the origin of this rectangle
     * @arg size A `Vector2` with the size of this rectangle
     *
     * @returns A new `Rect` instance
     */

    create = func rectCreate |origin, size| => {
        origin = origin ?? Vector2.create();
        size = size ?? Vector2.create();

        return { 
            origin = Vector2.create(origin.x, origin.y),
            size = Vector2.create(size.x, size.y)
        } @ Rect;
    },

    /***
     * If the rhs is another `Rect` the operation returns a new
     * `Rect` from the sum of both the origin and the size of the two
     * operands.
     * 
     * If the rhs is a `Vector2` the operation returns a new `Rect`
     * translated by that vector.
     */

    [object.symbols.sumOperator] = |other| -> {
        if (@other == Rect)
        {
            return Rect.create(
                this.origin + other.origin, 
                this.size + other.size
            );
        }
        else if (@other == Vector2)
        {
            return Rect.create(
                this.origin + other,
                this.size
            );
        }

        return false;
    },

    /***
     * Returns a new `Rect` with both the origin and the size
     * multiplied by the rhs value.
     */

    [object.symbols.mulOperator] = |other| -> {
        return Rect.create(
            this.origin * other, 
            this.size * other
        );
    },

    /***
     * Returns a new `Rect` with both the origin and the size
     * divided by the rhs value.
     */

    [object.symbols.divOperator] = |other| -> {
        return Rect.create(
            this.origin / other, 
            this.size / other
        );
    },

    /***
     * Returns the area of this `Rect`
     */

    [object.symbols.sizeOperator] = -> {
        return this.size.x * this.size.y;
    },
    
    [object.symbols.compareOperator] = |other| -> {
        if (!types.isObject(other))
        {
            return 1;
        }
        
        if (
            this.origin == other.origin 
            && this.size == other.size
        )
        {
            return 0;
        }

        return math.round(#this - #other);
    }
};

return Rect;