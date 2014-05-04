in vec4 position;

uniform mat4 _mvProj;
uniform vec4 _gameObjectUID;

out vec4 gameObjectUID;

void main(void) {
    // compute position
    gl_Position = _mvProj * position;

    gameObjectUID = _gameObjectUID;
}