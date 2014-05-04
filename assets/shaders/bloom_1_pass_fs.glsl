in vec2 vUv;

out vec4 fragColor;

uniform sampler2D mainTexture;

const float LumThresh = 0.75;

float luma( vec3 color ) {

  return 0.2126 * color.r + 0.7152 * color.g +0.0722 * color.b;
}

void main(void)
{
    vec4 val = vec4(texture(mainTexture,vUv).xyz,1.0);
    float weight =  clamp( luma(val.rgb) - LumThresh, 0.0, 1.0 ) * (1.0 / (1.0 - LumThresh) );

    fragColor =  val * weight;
}