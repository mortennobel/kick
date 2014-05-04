in vec4 position;
in vec3 normal;
in vec2 uv1;

uniform mat4 _mvProj;
uniform mat4 _mv;
uniform mat3 _norm;

out vec2 vUv;
out vec3 vNormal;
out vec3 vEcPosition;

void main(void) {
    vec4 v = position;
    // compute position
    gl_Position = _mvProj * v;

    vEcPosition = (_mv * v).xyz;
    vUv = uv1;
    // compute light info
    vNormal= normalize(_norm * normal);
} 