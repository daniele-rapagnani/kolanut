#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

/***
 * @module
 * 
 * This class represents a 4x4 matrix and can
 * be used to express 3D affine transformations.
 */

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

/***
 * Creates a new Transform3D intialized
 * with the identity matrix.
 * 
 * @returns A new Transform3D instance
 */

static TByte create(VM* vm)
{
    std::shared_ptr<Transform3D> mat = 
        std::make_shared<Transform3D>(1.0f)
    ;

    ffi::pushInstance(vm, "Kolanut", "Transform3D", mat);

    return 1;
}

/***
 * Sets this Transform3D to identity.
 */

static TByte identity(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    *mat = Transform3D { 1.0f };

    return 0;
}

/***
 * Multiplies this matrix by a new rotation matrix.
 * 
 * @arg angle The angle in degrees
 * @arg ?axis A `Vector3` with the axis. Defaults to (0, 0, 1).
 */

static TByte rotate(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, angleVal, MELON_TYPE_NUMBER, 0);
    melM_argOptional(vm, axisVal, MELON_TYPE_OBJECT, 1);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    glm::vec3 axis = { 0.0f, 0.0f, 1.0f };
    float angle = 0.0f;

    ffi::convert(vm, axis, axisVal);
    ffi::convert(vm, angle, angleVal);

    *mat = glm::rotate(
        *mat,
        angle,
        axis
    );

    return 0;
}

/***
 * Multiplies this matrix by a new translation matrix.
 * 
 * @arg amount A `Vector2` or a `Vector3` with the translation.
 */

static TByte translate(VM* vm)
{
    melM_this(vm, thisObj);
    melM_argOptional(vm, amountVal, MELON_TYPE_OBJECT, 0);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    glm::vec3 amount;
    Vec2f amount2;

    if (!ffi::convert(vm, amount, amountVal))
    {
        ffi::convert(vm, amount2, amountVal);
        amount = { amount2.x, amount2.y, 1.0f };
    }

    *mat = glm::translate(*mat, amount);

    return 0;
}

/***
 * Multiplies this matrix by a new scale matrix.
 * 
 * @arg amount A `Vector2` or a `Vector3` with the scale. A `Vector2` will only scale `x` and `y`.
 */

static TByte scale(VM* vm)
{
    melM_this(vm, thisObj);
    melM_argOptional(vm, amountVal, MELON_TYPE_OBJECT, 0);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    glm::vec3 amount;
    Vec2f amount2;

    if (!ffi::convert(vm, amount, amountVal))
    {
        ffi::convert(vm, amount2, amountVal);
        amount = { amount2.x, amount2.y, 1.0f };
    }

    *mat = glm::scale(*mat, amount);
    return 0;
}

/***
 * Creates a copy of this `Transform3D`s
 * 
 * @returns A new `Transform3D` instance
 */

static TByte copy(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;
    
    std::shared_ptr<Transform3D> mat2 = std::make_shared<Transform3D>(*mat);
    ffi::pushInstance(vm, "Kolanut", "Transform3D", mat2);

    return 1;
}

/***
 * Transposes this transform matrix.
 */

static TByte transpose(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    *mat = glm::transpose(*mat);

    return 0;
}

/***
 * Inverts this transform matrix.
 */

static TByte inverse(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    *mat = glm::inverse(*mat);

    return 0;
}

/***
 * Calculates the determinant of this 4x4 matrix.
 * 
 * @returns A scalar with the result of the determinant
 */

static TByte determinant(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, glm::determinant(*mat));

    return 1;
}

/***
 * Multiplies this `Transform3D` with another `Transform3D`, 
 * a `Vector4` or a `Vector3`.
 * 
 * If this matrix is multiplied by a vector a new transformed
 * `Vector3` or `Vector4` is returned. If the right hand side was a `Vector3`
 * the `w` component is assumed to be `1.0` and a `Vector3` is returned.
 * 
 * If this matrix is multiplied by another `Transform3D` the
 * result is another `Transform3D` with the result of the multiplication.
 * 
 * @returns Anotther
 */

static TByte opMultiply(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, otherVal, MELON_TYPE_OBJECT, 0);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    if (ffi::isa(vm, otherVal->pack.obj, "Kolanut", "Transform3D"))
    {
        std::shared_ptr<Transform3D> other = 
            ffi::getInstance<Transform3D>(vm, otherVal->pack.obj)
        ;

        std::shared_ptr<Transform3D> res = 
            std::make_shared<Transform3D>(1.0f)
        ;

        *res = glm::operator*(*mat, *other);

        ffi::pushInstance(vm, "Kolanut", "Transform3D", res);

        return 1;
    }

    glm::vec4 vec4;
    glm::vec3 vec3;

    if (ffi::convert(vm, vec4, otherVal))
    {
        glm::vec4 res = *mat * vec4;
        ffi::push(vm, res);
    }
    else if (ffi::convert(vm, vec3, otherVal))
    {
        glm::vec4 res = *mat * glm::vec4 { vec3.x, vec3.y, vec3.z, 1.0f };
        ffi::push(vm, glm::vec3 { res.x, res.y, res.z });
    }
    else
    {
        melM_vstackPushNull(&vm->stack);
    }

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "create", 0, 0, &create, 0 },
    { "copy", 1, 0, &copy, 1 },
    { "identity", 1, 0, &identity, 1 },
    { "rotate", 3, 0, &rotate, 1 },
    { "translate", 2, 0, &translate, 1 },
    { "scale", 2, 0, &scale, 1 },
    { "transpose", 1, 0, &transpose, 1 },
    { "inverse", 1, 0, &inverse, 1 },
    { "determinant", 1, 0, &determinant, 1 },
    { NULL, 2, 0, &opMultiply, 1, 0, MELON_OBJSYM_MUL },
    { NULL, 0, 0, NULL }
};

}

void registerTransform3DBindings(VM* vm)
{
    ffi::newSubmodule(vm, "Kolanut", "Transform3D", funcs);
}

} // namespace binding
} // namespace melon
} // namespace kola