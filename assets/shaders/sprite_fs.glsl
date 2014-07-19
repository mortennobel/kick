in vec2 vUv;

out vec4 fragColor;

uniform sampler2D mainTexture;
in vec4 vColor;

void main(void)
{
    fragColor = texture(mainTexture,vUv) * vColor;

}
