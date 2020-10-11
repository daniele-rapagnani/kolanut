let Assets = {
    Fonts = {
        Title = null
    },

    load = => {
        Assets.Fonts.Title = Kolanut.loadFont("assets/Pokemon Classic.ttf", 48);
    }
};

return Assets;