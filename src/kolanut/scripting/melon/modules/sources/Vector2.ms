/***
 * @module
 * 
 * This class represents a vector in 2D space
 */

let Vector2 = {

    /***
     * Creates a new `Vector2`
     *
     * @arg x The x component
     * @arg y The y component
     *
     * @returns A new instance of `Vector2`
     */

    create = func vector2Create |x, y| => {
        return { 
            x = number.fromNumber(x ?? 0), 
            y = number.fromNumber(y ?? 0)
        } @ Vector2;
    },

    /***
     * Sums two vectors
     */
    [object.symbols.sumOperator] = |other| -> {
        return Vector2.create(this.x + other.x, this.y + other.y);
    },

    /***
     * Subtracts two vectors
     */
    [object.symbols.subOperator] = |other| -> {
        return Vector2.create(this.x - other.x, this.y - other.y);
    },

    /***
     * Scalar product
     */
    [object.symbols.mulOperator] = |other| -> {
        return Vector2.create(this.x * other, this.y * other);
    },

    /***
     * Scalar division
     */
    [object.symbols.divOperator] = |other| -> {
        return Vector2.create(this.x / other, this.y / other);
    },

    /***
     * Returns the vector's magnitude
     */
    [object.symbols.sizeOperator] = -> {
        return math.sqrt(this.x * this.x + this.y * this.y);
    },

    /***
     * Returns a negated copy of this vector
     */
    [object.symbols.negOperator] = -> {
        return Vector2.create(-this.x, -this.y);
    },

    /***
     * Allows access of the components of this vector by numeric index
     */
    [object.symbols.getIndexOperator] = |idx| -> {
        if (idx == 0) {
            return this.x;
        } else if (idx == 1) {
            return this.y;
        }

        return false;
    },

    /***
     * Allows setting the components of this vector by numeric index
     */
    [object.symbols.setIndexOperator] = |idx, val| -> {
        if (!types.isInteger(idx)) {
            return false;
        }

        if (idx == 0) {
            this.x = val;
        } else if (idx == 1) {
            this.y = val;
        }

        return idx == 0 || idx == 1;
    },

    /***
     * Compares two vectors returning 0 if they match exactly or
     * the difference of their magnitude otherwise.
     */
    [object.symbols.compareOperator] = |other| -> {
        if (!types.isObject(other)) {
            return 1;
        }
        
        if (this.x == other.x && this.y == other.y) {
            return 0;
        }

        return math.round(#this - #other);
    },

    /***
     * Dot product
     */
    [object.symbols.powOperator] = |other| -> {
        return this.x * other.x + this.y * other.y;
    }
};

return Vector2;