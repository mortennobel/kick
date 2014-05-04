in vec2 vUv;
in vec4 vColor;

uniform vec4 mainColor = vec4(1.0,1.0,1.0,1.0);
uniform sampler2D mainTexture;

out vec4 fragColor;

void main(void)
{
    fragColor = vec4(texture(mainTexture,vUv).xyz*mainColor.xyz*vColor.xyz,1.0);
}
 