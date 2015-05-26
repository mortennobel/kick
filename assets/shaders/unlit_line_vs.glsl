in vec4 position;
in vec2 uv1;

uniform mat4 _mvProj;
uniform float zOffset;

out vec2 vUv;

void main(void) {
    gl_Position = _mvProj * position + vec4(0,0,zOffset,0);
    vUv = uv1;
}