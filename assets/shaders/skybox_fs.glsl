uniform vec4 mainColor = vec4(1.0,1.0,1.0,1.0);
uniform samplerCube mainTexture;

in vec3 vPos;
out vec4 fragColor;

void main(void)
{
    fragColor =  texture(mainTexture,vPos)*mainColor;
}