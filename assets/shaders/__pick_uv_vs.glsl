in vec4 position;
in vec2 uv1;

uniform mat4 _mvProj;
uniform mat3 _norm;

out vec2 vUV;

void main(void) {
    // compute position
    gl_Position = _mvProj * position;
    vUV = uv1;
}