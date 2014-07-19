in vec4 position;
in vec2 uv1;
in vec4 color;

uniform mat4 _mvProj;

out vec2 vUv;
out vec4 vColor;

void main(void) {
    gl_Position = _mvProj * position;
    vUv = uv1;
    vColor = color;
}