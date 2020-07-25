let Vector = {
    create = |x, y| => {
        return { 
            x = number.fromNumber(x), 
            y = number.fromNumber(y) 
        } @ Vector;
    },
    [object.symbols.sumOperator] = |other| -> {
        return Vector.create(this.x + other.x, this.y + other.y);
    },
    [object.symbols.subOperator] = |other| -> {
        return Vector.create(this.x - other.x, this.y - other.y);
    },
    [object.symbols.mulOperator] = |other| -> {
        return Vector.create(this.x * other, this.y * other);
    },
    [object.symbols.divOperator] = |other| -> {
        return Vector.create(this.x / other, this.y / other);
    },
    [object.symbols.sizeOperator] = -> {
        return math.sqrt(this.x * this.x + this.y * this.y);
    },
    [object.symbols.negOperator] = -> {
        return Vector.create(-this.x, -this.y);
    },
    [object.symbols.getIndexOperator] = |idx| -> {
        if (idx == 0)
        {
            return this.x;
        }
        else if (idx == 1)
        {
            return this.y;
        }

        return false;
    },
    [object.symbols.setIndexOperator] = |idx, val| -> {
        if (!types.isInteger(idx))
        {
            return false;
        }

        if (idx == 0)
        {
            this.x = val;
        }
        else if (idx == 1)
        {
            this.y = val;
        }

        return idx == 0 || idx == 1;
    },
    [object.symbols.compareOperator] = |other| -> {
        if (!types.isObject(other))
        {
            return 1;
        }
        
        if (this.x == other.x && this.y == other.y)
        {
            return 0;
        }

        return math.round(#this - #other);
    },
    [object.symbols.powOperator] = |other| -> {
        return this.x * other.x + this.y * other.y;
    }
};

return Vector;