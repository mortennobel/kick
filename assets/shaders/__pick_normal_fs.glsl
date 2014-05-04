in vec3 vNormal;

out vec4 fragColor;

void main(void)
{
    fragColor = vec4(vNormal,0);
}