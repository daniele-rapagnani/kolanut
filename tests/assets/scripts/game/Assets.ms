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
        Jump = null,
        Land = null,
        Hit = null,
        Death = null,
        Attack = null,
        Music = null
    },

    load = => {
        Assets.Fonts.Pixel = Kolanut.loadFont("assets/Pokemon Classic.ttf", 48);

        Assets.Sounds.Jump = Kolanut.loadSound("assets/jump.wav");
        Assets.Sounds.Land = Kolanut.loadSound("assets/land.wav");
        Assets.Sounds.Hit = Kolanut.loadSound("assets/hit.wav");
        Assets.Sounds.Death = Kolanut.loadSound("assets/death.wav");
        Assets.Sounds.Attack = Kolanut.loadSound("assets/attack.wav");
        Assets.Sounds.Music = Kolanut.loadSound("assets/music.wav");

        Assets.Sprites.Hero = Kolanut.loadSprite("assets/herochar_spritesheet.png");
        Assets.Sprites.Goblin = Kolanut.loadSprite("assets/goblin_spritesheet.png");
        
        Assets.UI.Heart = Kolanut.loadSprite("assets/hearts_hud.png");
        Assets.UI.NoHeart = Kolanut.loadSprite("assets/no_hearts_hud.png");
    }
};

return Assets;