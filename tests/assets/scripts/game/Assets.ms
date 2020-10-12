let Assets = {
    Fonts = {
        Title = null
    },

    Sprites = {
        Hero = null,
        Goblin = null
    },

    load = => {
        Assets.Fonts.Title = Kolanut.loadFont("assets/Pokemon Classic.ttf", 48);
        Assets.Sprites.Hero = Kolanut.loadSprite("assets/herochar_spritesheet.png");
        Assets.Sprites.Goblin = Kolanut.loadSprite("assets/goblin_spritesheet.png");
    }
};

return Assets;