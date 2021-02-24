#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, binding = 2) uniform ViewInfo
{
    vec2 screenSize;
    mat4 transform;
    mat4 camera;
};

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;

layout(location = 0) out vec2 v_texCoord;
layout(location = 1) out vec4 v_vertColor;

void main() {
    mat4 mv = camera * transform;
    vec4 pos = mv * vec4(a_position.xy, 0.0, 1.0);
    pos.xy /= screenSize;
    pos.xy -= vec2(1.0, 1.0);

    gl_Position = vec4(pos.xy, 0.0, 1.0);
    v_texCoord = a_texCoord;
    v_vertColor = a_color;
}