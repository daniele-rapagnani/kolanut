#ifdef __EMSCRIPTEN__
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#else
#include <glad/glad.h>
#endif