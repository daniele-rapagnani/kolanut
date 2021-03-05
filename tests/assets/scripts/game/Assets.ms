let Assets = {
    Fonts = {
        Title = null,
        Small = null
    },

    Sprites = {
        Hero = null,
        Goblin = null
    },

    UI = {
        Heart = null,
        NoHeart = null
    },

    Sounds = {
        Beep = null
    },

    load = => {
        Assets.Fonts.Pixel = Kolanut.loadFont("assets/Pokemon Classic.ttf", 48);

        Assets.Sounds.Beep = Kolanut.loadSound("assets/beep.mp3");

        Assets.Sprites.Hero = Kolanut.loadSprite("assets/herochar_spritesheet.png");
        Assets.Sprites.Goblin = Kolanut.loadSprite("assets/goblin_spritesheet.png");
        
        Assets.UI.Heart = Kolanut.loadSprite("assets/hearts_hud.png");
        Assets.UI.NoHeart = Kolanut.loadSprite("assets/no_hearts_hud.png");
    }
};

return Assets;