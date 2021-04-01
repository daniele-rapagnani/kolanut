
# Class BoundingBox



This class represents a 2D axis aligned bounding box.


## Functions

### create(left, top, right, bottom)

Creates a new BoundingBox object     * 
| Argument | Description |
| -------- | ----------- |
|  **left**  | The min x of this AABB ||  **top**  | The min y of this AABB ||  **right**  | The max x of this AABB ||  **bottom**  | The max y of this AABB |


**Returns:** A new BoundingBox object

### createFromPos(position, size)

Creates a new BoundingBox given a position and a size   * 
| Argument | Description |
| -------- | ----------- |
|  **position**  | A `Vector2` with the AABB position ||  **size**  | A `Vector2` holding the AABB's size |


**Returns:** A new BoundingBox object

### createFromRect(rect)

Creates a new BoundingBox from a given `Rect`.  * 
| Argument | Description |
| -------- | ----------- |
|  **rect**  | A `Rect` holding the coordinates of this AABB. |


**Returns:** A new BoundingBox object




## Methods

### getSize()

Gets the size of this AABB. * 


**Returns:** A `Vector2` holding the size of this AABB

### getOrigin()

Gets the origin of this AABB. * 


**Returns:** A `Vector2` holding the origin of this AABB

### overlaps(bb)

Checks whether this AABB overlaps another. *  * 
| Argument | Description |
| -------- | ----------- |
|  **bb**  | The other AABB that may overlap this. |


**Returns:** `true` if they overlap, `false` otherwise

### contains(bb)

Checks whether this AABB contains another. *  * 
| Argument | Description |
| -------- | ----------- |
|  **bb**  | The AABB that may be contained by this one. |


**Returns:** `true` if this ABBB contains *bb*, `false` otherwise




## Operators

The class supports the following operators:

| Operator | Description |
| -------- | ----------- |
| operator + | If the rhs object is another BoundingBox the result will be the union of the two AABBs. If the rhs object is a Vector2 the result will be a new AABB translated by that vector. |
| operator * | If the rhs object is a `Transform3D` then the result will be the resulting transformed AABB. If the rhs value it will be multiplied to the size of the lhs AABB and a new AABB will be returned. |
| operator # | Returns the area of this AABB |
| operator &lt;&gt; |  |



