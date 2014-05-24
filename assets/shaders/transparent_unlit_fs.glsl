in vec2 vUv;

out vec4 fragColor;

uniform vec4 mainColor;
uniform sampler2D mainTexture;

void main(void)
{
    fragColor = texture(mainTexture,vUv) * mainColor;
}
