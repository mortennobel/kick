in vec2 vUv;
in vec3 vNormal;
in vec3 vEcPosition;
in vec4 vShadowMapCoord;
in vec4 vColor;

out vec4 fragColor;

uniform vec4 mainColor;
uniform sampler2D mainTexture;

#pragma include "assets/shaders/light.glsl"
#pragma include "assets/shaders/shadowmap.glsl"

void main(void)
{
    vec3 normal = normalize(vNormal);
    vec3 directionalLight = getDirectionalLightDiffuse(normal,_dLight);
    vec3 pointLight = getPointLightDiffuse(normal,vEcPosition, _pLights);
    float visibility;
    if (SHADOWS){
        visibility = computeLightVisibility(vShadowMapCoord);
    } else {
        visibility = 1.0;
    }
    vec3 color = max((directionalLight+pointLight)*visibility,_ambient.xyz) * vColor.xyz * mainColor.xyz;

    fragColor = vec4(texture(mainTexture,vUv).xyz*color, 1.0);
}
 