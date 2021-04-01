
# Class Tilemap



This class represents a Tiled tilemap


## Functions

### create()

Creates a new `Tilemap` instance * 


**Returns:** A new `Tilemap` object




## Methods

### load(assetDir, mapFile)

Loads a tilemap from file into this instance.   
| Argument | Description |
| -------- | ----------- |
|  **assetDir**  | The directory in which the tilemap is located ||  **mapFile**  | The filename of the tilemap to load |


**Returns:** `true` on success, `false` otherwise

### instantiateLayer(layerNum, position, [scale])

Instantiate a specific layer. Only instantiated layers will be drawn when this tilemap is drawn.   
| Argument | Description |
| -------- | ----------- |
|  **layerNum**  | The number of the layer to instantiate ||  **position**  | A `Vector2` with the position where the layer should be istantiated ||  **scale** &nbsp; <sub><sup>Optional</sup></sub>  | The scale at which the layer should be instantiated |


### instantiate(layerNum, position, [scale])

Instantiate a specific layer. Only instantiated layers will be drawn when this tilemap is drawn.   
| Argument | Description |
| -------- | ----------- |
|  **layerNum**  | The number of the layer to instantiate ||  **position**  | A `Vector2` with the position where the layer should be istantiated ||  **scale** &nbsp; <sub><sup>Optional</sup></sub>  | The scale at which the layer should be instantiated |


### draw()

Draw all instantiated layers at their respective positions and scales


### getLayersCount()

Gets the number of layers in this tilemap. 


**Returns:** The number of layers in this tilemap.

### getObjectByName(group, name)

Gets an object in this tilemap. The resulting object will be an object with the following structure: ``` { name: ... type: ... visible: ... x: ... y: ... w: ... h: ... } ```   
| Argument | Description |
| -------- | ----------- |
|  **group**  | The group of which this object is part of ||  **name**  | The name of the object you are looking for |


**Returns:** An object or `null`

### getObjectsByType(group, type)

Gets an objects in this tilemap by type. See `getObjectByName` for the result's structure.   
| Argument | Description |
| -------- | ----------- |
|  **group**  | The parent group of the objects you're looking fore ||  **type**  | The type of the objects you are looking for |


**Returns:** An array of objects

### getTileAt(layer, pos)

Gets a tile by position. If a tile is found an object with the following structure is returned: ``` { gid: ... id: ... position: ... rect: ... } ```   
| Argument | Description |
| -------- | ----------- |
|  **layer**  | The layer on which to check for tiles at the specific position ||  **pos**  | A `Vector2` with the position where you want to check for tiles |


**Returns:** A an object representing the tile or `null` if no tile is present





