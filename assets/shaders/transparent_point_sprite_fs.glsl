uniform sampler2D mainTexture;
uniform vec4 mainColor;

out vec4 fragColor;

void main(void)
{
	vec2 UVflippedY = gl_PointCoord;
	UVflippedY.y = 1.0 - UVflippedY.y;
    fragColor = texture(mainTexture, UVflippedY) * mainColor;
}