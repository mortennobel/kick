#pragma include "assets/shaders/shadowmap.glsl"

out vec4 fragColor;

void main() {
    fragColor = packDepth( gl_FragCoord.z );
}
 