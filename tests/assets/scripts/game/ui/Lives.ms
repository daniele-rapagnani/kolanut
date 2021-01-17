let Assets = import("../Assets");

let Lives = {
    draw = |pos, state| => {
        let sprite = null;
        let scale = Vector.create(1.0, 1.0);
        let padding = 5.0;

        for (let i in 0..(state.total ?? 3)) {
            if (i + 1 <= (state.count ?? 3)) {
                sprite = Assets.UI.Heart;
            } else {
                sprite = Assets.UI.NoHeart;
            }
            
            let sz = sprite->getSize();

            sprite->draw(
                pos + Vector.create((sz.x * scale.x * i) + (padding * i), 0.0),
                0.0,
                scale,
                Vector.create(0.0, 0.0),
                Rect.create(0, 0, sz.x, sz.y),
                Color.white
            );
        }
    }
};

return Lives;