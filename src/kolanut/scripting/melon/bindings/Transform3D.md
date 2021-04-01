
# Class Transform3D



This class represents a 4x4 matrix and can
be used to express 3D affine transformations.


## Functions

### create()

Creates a new Transform3D intialized with the identity matrix. 


**Returns:** A new Transform3D instance




## Methods

### copy()

Creates a copy of this `Transform3D`s 


**Returns:** A new `Transform3D` instance

### identity()

Sets this Transform3D to identity.


### rotate(angle, [axis])

Multiplies this matrix by a new rotation matrix.  
| Argument | Description |
| -------- | ----------- |
|  **angle**  | The angle in degrees ||  **axis** &nbsp; <sub><sup>Optional</sup></sub>  | A `Vector3` with the axis. Defaults to (0, 0, 1). |


### translate(amount)

Multiplies this matrix by a new translation matrix. 
| Argument | Description |
| -------- | ----------- |
|  **amount**  | A `Vector2` or a `Vector3` with the translation. |


### scale(amount)

Multiplies this matrix by a new scale matrix. 
| Argument | Description |
| -------- | ----------- |
|  **amount**  | A `Vector2` or a `Vector3` with the scale. A `Vector2` will only scale `x` and `y`. |


### transpose()

Transposes this transform matrix.


### inverse()

Inverts this transform matrix.


### determinant()

Calculates the determinant of this 4x4 matrix. 


**Returns:** A scalar with the result of the determinant




## Operators

The class supports the following operators:

| Operator | Description |
| -------- | ----------- |
| operator * | Multiplies this `Transform3D` with another `Transform3D`, a `Vector4` or a `Vector3`. If this matrix is multiplied by a vector a new transformed `Vector3` or `Vector4` is returned. If the right hand side was a `Vector3` the `w` component is assumed to be `1.0` and a `Vector3` is returned. If this matrix is multiplied by another `Transform3D` the result is another `Transform3D` with the result of the multiplication.  |



