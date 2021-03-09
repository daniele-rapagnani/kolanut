#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

static TByte create(VM* vm)
{
    std::shared_ptr<Transform3D> mat = 
        std::make_shared<Transform3D>(1.0f)
    ;

    ffi::pushInstance(vm, "Kolanut", "Transform3D", mat);

    return 1;
}

static TByte identity(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    *mat = Transform3D { 1.0f };

    return 0;
}

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

static TByte translate(VM* vm)
{
    melM_this(vm, thisObj);
    melM_argOptional(vm, amountVal, MELON_TYPE_OBJECT, 1);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    glm::vec3 amount;

    ffi::convert(vm, amount, amountVal);

    *mat = glm::translate(*mat, amount);

    return 0;
}

static TByte scale(VM* vm)
{
    melM_this(vm, thisObj);
    melM_argOptional(vm, amountVal, MELON_TYPE_OBJECT, 1);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    glm::vec3 amount;

    ffi::convert(vm, amount, amountVal);

    *mat = glm::scale(*mat, amount);

    return 0;
}

static TByte transpose(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    *mat = glm::transpose(*mat);

    return 0;
}

static TByte determinant(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<Transform3D> mat = 
        ffi::getInstance<Transform3D>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, glm::determinant(*mat));

    return 1;
}

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

    // glm::vec4 vec;

    // if (ffi::convert(vm, vec, otherVal))
    // {
    //     glm::vec4 res = glm::operator*(*mat, vec);
    //     ffi::push(res);
    // }

    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "create", 0, 0, &create, 0 },
    { "identity", 1, 0, &identity, 1 },
    { "rotate", 3, 0, &rotate, 1 },
    { "translate", 2, 0, &translate, 1 },
    { "scale", 2, 0, &scale, 1 },
    { "transpose", 1, 0, &transpose, 1 },
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