uniform vec2 screenSize;
uniform mat3 transform;
uniform mat3 camera;

attribute vec2 a_position;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;

void main() {
    mat3 mv = transform * camera;
    vec3 pos = vec3(a_position.xy, 1.0) * mv;

    pos.xy /= screenSize;
    pos.xy -= vec2(1.0, 1.0);
    pos.y *= -1.0;

    gl_Position.xy = pos.xy;
    gl_Position.zw = vec2(0.0, 1.0);
    v_texCoord = a_texCoord;
}