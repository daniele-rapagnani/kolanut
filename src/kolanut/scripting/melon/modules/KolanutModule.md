
# Module KolanutModule



This module exposes basic APIs that can be used to
interact with he Kolanut engine.


## Functions

### onLoad()

Called when the assets should be loaded


### onUpdate(dt)

Called when the game should be updated 
| Argument | Description |
| -------- | ----------- |
|  **dt**  | The current delta time |


### onDraw()

Called when the current frame should be drawn


### onDrawUI()

Called when the current frame should be drawn


### onQuit()

Called when the user tries to close the game. 


**Returns:** `true` if the game should close, `false` if it shouldn&#39;t.

### onKeyPressed()

Called when the user tries to close the game. 


**Returns:** `true` if the game should close, `false` if it shouldn&#39;t.

### loadSprite(path)

Loads a sprite image from a file  
| Argument | Description |
| -------- | ----------- |
|  **path**  | The path of the sprite to load |


**Returns:** A valid sprite reference if successfull or `null`

### loadFont(path, [size])

Loads a font from a file   
| Argument | Description |
| -------- | ----------- |
|  **path**  | The path of the font to load ||  **size** &nbsp; <sub><sup>Optional</sup></sub>  | The size at which this font will be rendered to bitmap, defaults to 16px |


**Returns:** A valid font reference if successfull or `null`

### loadSound(path)

Loads sound from a file  
| Argument | Description |
| -------- | ----------- |
|  **path**  | The path of the sound to load |


**Returns:** A valid sound reference if successfull or `null`

### setMasterVolume(gain)

Sets the master audio volume, setting this to `0.0` will mute all audio. 
| Argument | Description |
| -------- | ----------- |
|  **gain**  | The gain to set, a number between `0.0` and `1.0` |


### getMasterVolume()

Gets the current master volume's gain. 


**Returns:** The current master volume&#39;s gain, a number between `0.0` and `1.0`

### getScreenSize()

Gets the size of the window in which the game is rendered (or the screen if it's fullscreen). 


**Returns:** A Vector2 with the current size

### getDesignResolution()

Gets the size the game currently uses to render its content and that its adapted to the actual window/screen size. 


**Returns:** A Vector2 with the current size

### getTime()

Gets the current time since the game started in milliseconds. 


**Returns:** Returns an integer representing the number of milliseconds passed.

### drawRect()

Draws the wireframe of a rectangle on the screen using the current camera settings. @args rect A `Rect`, the one to draw @args ?color The `Color` to use when drawing the rect. Defaults to white.


### drawLine()

Draws a line on the screen using the current camera settings. @args a A `Vector2` with the first point's position @args b A `Vector2` with the second point's position @args ?color The `Color` to use when drawing the rect. Defaults to white.








