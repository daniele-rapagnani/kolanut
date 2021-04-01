/***
 * @module
 * 
 * This class represents RGBA8 colors
 * 
 * @exports Color.white A costant for the white color
 * @exports Color.black A constant for the black color
 * @exports Color.red A constant for the red color
 * @exports Color.green A constant for the green color
 * @exports Color.blue A constant for the blue color
 * @exports Color.magenta A constant for the magenta color
 * @exports Color.yellow A constant for the yellow color
 * @exports Color.cyan A constant for the cyan color
 */

let Color = {
    /***
     * Creates a new `Color` object.
     * 
     * @arg r The red component 0-255
     * @arg g The green component 0-255
     * @arg b The blue component 0-255
     * @arg a The alpha component 0-255
     * 
     * @returns A new `Color` instance
     */

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