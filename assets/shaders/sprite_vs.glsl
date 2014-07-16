in vec4 position;
in vec2 uv1;

uniform mat4 _mvProj;

out vec2 vUv;

void main(void) {
    gl_Position = _mvProj * position;
    vUv = uv1;
}