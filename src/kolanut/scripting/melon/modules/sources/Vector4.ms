/***
 * @module
 * 
 * This class represents a vector in 4D or projective space
 */

let Vector4 = {
    /***
     * Creates a new `Vector4`
     *
     * @arg x The x component
     * @arg y The y component
     * @arg z The z component
     *
     * @returns A new instance of `Vector4`
     */

    create = func vector4Create |x, y, z, w| => {
        return { 
            x = number.fromNumber(x ?? 0), 
            y = number.fromNumber(y ?? 0),
            z = number.fromNumber(z ?? 0),
            w = number.fromNumber(w ?? 0)
        } @ Vector4;
    },

    /***
     * Sums two vectors
     */
    [object.symbols.sumOperator] = |other| -> {
        return Vector4.create(this.x + other.x, this.y + other.y, this.z + other.z, this.w + other.w);
    },

    /***
     * Subtracts two vectors
     */
    [object.symbols.subOperator] = |other| -> {
        return Vector4.create(this.x - other.x, this.y - other.y, this.z - other.z, this.w - other.w);
    },

    /***
     * Scalar product
     */
    [object.symbols.mulOperator] = |other| -> {
        return Vector4.create(this.x * other, this.y * other, this.z * other, this.w * other);
    },

    /***
     * Scalar division
     */
    [object.symbols.divOperator] = |other| -> {
        return Vector4.create(this.x / other, this.y / other, this.z / other, this.w / other);
    },

    /***
     * Returns the vector's magnitude
     */
    [object.symbols.sizeOperator] = -> {
        return math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z + this.w * this.w);
    },

    /***
     * Returns a negated copy of this vector
     */
    [object.symbols.negOperator] = -> {
        return Vector4.create(-this.x, -this.y, -this.z, -this.w);
    },

    /***
     * Allows access of the components of this vector by numeric index
     */
    [object.symbols.getIndexOperator] = |idx| -> {
        if (idx == 0) {
            return this.x;
        } else if (idx == 1) {
            return this.y;
        } else if (idx == 2) {
            return this.z;
        } else if (idx == 3) {
            return this.w;
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
        } else if (idx == 2) {
            this.z = val;
        } else if (idx == 3) {
            this.w = val;
        }

        return idx == 0 || idx == 1 || idx == 2 || idx == 3;
    },

    /***
     * Compares two vectors returning 0 if they match exactly or
     * the difference of their magnitude otherwise.
     */
    [object.symbols.compareOperator] = |other| -> {
        if (!types.isObject(other)) {
            return 1;
        }
        
        if (this.x == other.x && this.y == other.y && this.z == other.z && this.w == other.w) {
            return 0;
        }

        return math.round(#this - #other);
    },

    /***
     * Dot product
     */
    [object.symbols.powOperator] = |other| -> {
        return this.x * other.x + this.y * other.y + this.z * other.z + this.w * other.w;
    }
};

return Vector4;