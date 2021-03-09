let scale = Vector2.create(0.25, 0.25);
let DEF_MAX_WIDTH = 100;

let Stats = {
    create = |font, size, color, maxWidth| => {
        return {
            color = color ?? Color.white,
            maxWidth = maxWidth ?? DEF_MAX_WIDTH,
            size = size,
            font = font
        } @ Stats;
    },

    draw = |pos, stats| -> {
        let posText = Vector2.create(pos.x + this.maxWidth, pos.y);

        for (let k in stats) {
            this.font->draw(k, pos, this.size, color);

            this.font->draw(
                string.format("%.2f", stats[k].avg), 
                posText,
                this.size,
                color
            );

            pos.y = pos.y + this.size;
            posText.y = posText.y + this.size;
        }
    }
};

return Stats;