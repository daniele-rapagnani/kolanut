let Color = {
    create = |r, g, b, a| => {
        return { 
            r = r, 
            g = g, 
            b = b, 
            a = a
        } @ Color;
    }
};

Color.white = Color.create(255, 255, 255, 255);
Color.black = Color.create(0, 0, 0, 255);
Color.red = Color.create(255, 0, 0, 255);
Color.green = Color.create(0, 255, 0, 255);
Color.blue = Color.create(0, 0, 255, 255);
Color.magenta = Color.create(255, 0, 255, 255);
Color.yellow = Color.create(255, 255, 0, 255);
Color.cyan = Color.create(0, 255, 255, 255);

return Color;