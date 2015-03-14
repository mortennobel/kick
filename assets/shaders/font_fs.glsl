in vec2 vUv;

out vec4 fragColor;

uniform vec4 mainColor;
uniform sampler2D mainTexture;
uniform float distmin;
uniform float distmax;

void main(void)
{
    float distance = texture(mainTexture,vUv).r;
    float a = smoothstep(distmin, distmax, distance);
    vec4 baseColor = vec4(mainColor.rgb, mainColor.a * a);
    fragColor = baseColor;
}
 