in vec2 vUv;

out vec4 fragColor;

uniform vec4 mainColor = vec4(1.0,1.0,1.0,1.0);
uniform sampler2D mainTexture;

void main(void)
{
    fragColor = vec4( texture(mainTexture,vUv).xyz * mainColor.xyz, 1.0);
}
 