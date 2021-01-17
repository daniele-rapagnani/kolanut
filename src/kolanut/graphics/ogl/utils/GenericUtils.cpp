#include "kolanut/core/Logging.h"
#include "kolanut/graphics/ogl/utils/shaders/Program.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"

#include <cassert>

namespace kola {
namespace utils {
namespace ogl {

namespace {
    float quadBuffer[6 * 2];
}

void draw(QuadDrawRequest dr)
{
    assert(dr.program);
    assert(dr.texture);

    dr.program->use();
    
    if (!dr.program->setUnifrom("transform", dr.transform))
    {
        knM_logError("Can't find transform uniform in main shader");
        return;
    }

    GLint posAtt = dr.program->getAttributeLocation("a_position");

    if (posAtt < 0)
    {
        knM_logError("Can't find a_position attribute in main vertex shader");
        return;
    }

    GLint tcAtt = dr.program->getAttributeLocation("a_texCoord");

    if (tcAtt < 0)
    {
        knM_logError("Can't find a_texCoord attribute in main vertex shader");
        return;
    }

    GLuint buffers[2];
    knM_oglCall(glGenBuffers(2, buffers));

    knM_oglCall(glBindBuffer(GL_ARRAY_BUFFER, buffers[0]));
    knM_oglCall(glEnableVertexAttribArray(posAtt));
    knM_oglCall(glVertexAttribPointer(posAtt, 2, GL_FLOAT, GL_FALSE, 0, 0));

    Sizei size = dr.texture->getSize();

    Rectf tcRect = { 
        dr.textureRect.x / static_cast<float>(size.x), 
        dr.textureRect.y / static_cast<float>(size.y),
        0.0f,
        0.0f
    };
    
    tcRect.z = tcRect.x + (dr.textureRect.z / static_cast<float>(size.x));
    tcRect.w = tcRect.y + (dr.textureRect.w / static_cast<float>(size.y));

    quadBuffer[0] = 0;
    quadBuffer[1] = 0;

    quadBuffer[2] = dr.textureRect.z;
    quadBuffer[3] = 0;
    
    quadBuffer[4] = 0;
    quadBuffer[5] = dr.textureRect.w;
    
    quadBuffer[6] = 0;
    quadBuffer[7] = dr.textureRect.w;
    
    quadBuffer[8] = dr.textureRect.z;
    quadBuffer[9] = 0;
    
    quadBuffer[10] = dr.textureRect.z;
    quadBuffer[11] = dr.textureRect.w;

    knM_oglCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadBuffer), quadBuffer, GL_STREAM_DRAW));

    knM_oglCall(glBindBuffer(GL_ARRAY_BUFFER, buffers[1]));
    knM_oglCall(glEnableVertexAttribArray(tcAtt));
    knM_oglCall(glVertexAttribPointer(tcAtt, 2, GL_FLOAT, GL_FALSE, 0, 0));

    quadBuffer[0] = tcRect.x;
    quadBuffer[1] = tcRect.y;

    quadBuffer[2] = tcRect.z;
    quadBuffer[3] = tcRect.y;
    
    quadBuffer[4] = tcRect.x;
    quadBuffer[5] = tcRect.w;
    
    quadBuffer[6] = tcRect.x;
    quadBuffer[7] = tcRect.w;
    
    quadBuffer[8] = tcRect.z;
    quadBuffer[9] = tcRect.y;
    
    quadBuffer[10] = tcRect.z;
    quadBuffer[11] = tcRect.w;

    knM_oglCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadBuffer), quadBuffer, GL_STREAM_DRAW));

    dr.texture->bind();

    knM_oglCall(glDrawArrays(GL_TRIANGLES, 0, 6));

    knM_oglCall(glDeleteBuffers(2, buffers));
}

} // namespace ogl
} // namespace utils
} // namespace kola