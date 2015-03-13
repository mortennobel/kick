in vec2 vUv;

out vec4 fragColor;

uniform vec4 mainColor;
uniform sampler2D mainTexture;

void main(void)
{

    float smoothness = 0.5; // between 0.0 and 1.0
    float offset = smoothness*0.5;
    float a = smoothstep(0.5-offset, 0.5+offset, texture(mainTexture,vUv).a);
    vec4 baseColor = vec4(mainColor.rgb, mainColor.a * a);
    fragColor = baseColor;
}
 