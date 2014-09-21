in vec4 position;

uniform mat4 _mvProj;

void main(void) {
    // compute position
    gl_Position = _mvProj * position;
}