
# Class Font



This class represents an instance of a font.




## Methods

### draw(text, [position], [size], [color])

Draws some text using this font.    
| Argument | Description |
| -------- | ----------- |
|  **text**  | A string with the text to draw ||  **position** &nbsp; <sub><sup>Optional</sup></sub>  | A `Vector2` with the position where the text should be drawn ||  **size** &nbsp; <sub><sup>Optional</sup></sub>  | The size of the text in pixels, defaults to the size used when loading the font ||  **color** &nbsp; <sub><sup>Optional</sup></sub>  | The `Color` to use to draw |


### getTextSize(text)

Returns the size of some text rendered with this font at its native size.  
| Argument | Description |
| -------- | ----------- |
|  **text**  | The text of which you want to know the size when drawn |


**Returns:** A `Vector2` with the text size

### getNativeSize()

Gets the size at which this font has been rasterized, the same used to load the font. 


**Returns:** The size used to rasterize this font in pixels





