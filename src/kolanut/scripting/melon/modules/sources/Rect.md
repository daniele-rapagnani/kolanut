
# Module Rect



*
This class represents rectangles in 2D space


## Functions

### create(origin, size)

Creates a rectangle given an origin and a size *   * 
| Argument | Description |
| -------- | ----------- |
|  **origin**  | A `Vector2` pointing to the origin of this rectangle ||  **size**  | A `Vector2` with the size of this rectangle |


**Returns:** A new `Rect` instance






## Operators

The class supports the following operators:

| Operator | Description |
| -------- | ----------- |
| operator + | If the rhs is another `Rect` the operation returns a new `Rect` from the sum of both the origin and the size of the two operands. If the rhs is a `Vector2` the operation returns a new `Rect` translated by that vector. |
| operator * | Returns a new `Rect` with both the origin and the size multiplied by the rhs value. |
| operator / | Returns a new `Rect` with both the origin and the size divided by the rhs value. |
| operator # | Returns the area of this `Rect` |
| operator &lt;&gt; |  |



