let Rect = {
    create = |origin, size| => {
        return { 
            origin = Vector2.create(origin.x, origin.y),
            size = Vector2.create(size.x, size.y)
        } @ Rect;
    },
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
    [object.symbols.mulOperator] = |other| -> {
        return Rect.create(
            this.origin * other, 
            this.size * other
        );
    },
    [object.symbols.divOperator] = |other| -> {
        return Rect.create(
            this.origin / other, 
            this.size / other
        );
    },
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