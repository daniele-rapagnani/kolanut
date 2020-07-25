let Rect = {
    create = |x, y, w, h| => {
        return { 
            x = x, 
            y = y, 
            w = w, 
            h = h 
        } @ Rect;
    },
    [object.symbols.sumOperator] = |other| -> {
        if (@other == Rect)
        {
            return Rect.create(
                this.x + other.x, 
                this.y + other.y,
                this.w + other.w,
                this.h + other.h
            );
        }
        else if (@other == Vector)
        {
            return Rect.create(
                this.x + other.x, 
                this.y + other.y,
                this.w,
                this.h
            );
        }

        return false;
    },
    [object.symbols.mulOperator] = |other| -> {
        return Rect.create(
            this.x * other, 
            this.y * other, 
            this.w * other, 
            this.h * other
        );
    },
    [object.symbols.divOperator] = |other| -> {
        return Rect.create(
            this.x / other, 
            this.y / other, 
            this.w / other, 
            this.h / other
        );
    },
    [object.symbols.sizeOperator] = -> {
        return this.w * this.h;
    },
    [object.symbols.compareOperator] = |other| -> {
        if (!types.isObject(other))
        {
            return 1;
        }
        
        if (
            this.x == other.x 
            && this.y == other.y
            && this.w == other.w
            && this.h == other.h
        )
        {
            return 0;
        }

        return math.round(#this - #other);
    }
};

return Rect;