uniform vec4 mainColor;
uniform samplerCube mainTexture;

in vec3 vPos;
out vec4 fragColor;

void main(void)
{
    fragColor =  texture(mainTexture,vPos)*mainColor;
}