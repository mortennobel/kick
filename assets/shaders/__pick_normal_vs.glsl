in vec4 position;
in vec3 normal;

uniform mat4 _mvProj;
uniform mat3 _norm;

out vec3 vNormal;

void main(void) {
    // compute position
    gl_Position = _mvProj * position;
    vNormal = (_norm * normal) / 2.0 + vec3(0.5, 0.5, 0.5);
}