in vec2 vUv;

out vec4 fragColor;

uniform vec4 mainColor;
uniform sampler2D mainTexture;

const float gamma = 2.2;

void main(void)
{
    float a = smoothstep(0.25, 0.75, texture(mainTexture,vUv).a);
    vec4 baseColor = vec4(mainColor.rgb, mainColor.a * a);
    fragColor = baseColor;
}
 