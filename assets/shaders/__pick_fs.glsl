uniform vec2 _gameObjectUID;

out vec4 fragColor;

void main(void)
{
    fragColor = _gameObjectUID.xyxy;
}