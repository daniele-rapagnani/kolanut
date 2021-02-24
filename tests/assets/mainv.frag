#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 1) uniform sampler2D u_image;

layout(location = 0) in vec2 v_texCoord;
layout(location = 1) in vec4 v_vertColor;
layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = texture(u_image, v_texCoord) * v_vertColor;
}