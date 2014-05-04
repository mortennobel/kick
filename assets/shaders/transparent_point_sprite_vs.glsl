in vec4 position;

uniform mat4 _mvProj;
uniform float pointSize;

void main(void) {
	gl_Position = _mvProj * position;
    gl_PointSize = pointSize / gl_Position.w;
} 