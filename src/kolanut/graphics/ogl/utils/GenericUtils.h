#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/ogl/TextureOGL.h"

#include "kolanut/graphics/ogl/OpenGL.h"

#include <memory>

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
#define knM_oglCall(x) \
    do { \
        x; \
        { \
            GLenum e = glGetError(); \
            if (e != GL_NO_ERROR) \
                knM_logError("OpenGL call error: " << __FILE__ << ":" << __LINE__ << " - " << e); \
        } \
    } while(false)
#else
#define knM_oglCall(x) x;
#endif