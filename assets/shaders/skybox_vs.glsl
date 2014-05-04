in vec4 position;

uniform mat4 _mvProj;
uniform mat4 _v;

out vec3 vPos;

void main(void) {
    gl_Position = _mvProj * position;
    vPos = (position * _v).xyz; // inverse view direction * pos
}