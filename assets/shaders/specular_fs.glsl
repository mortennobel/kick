in vec2 vUv;
in vec3 vNormal;
in vec3 vEcPosition;
in vec4 vShadowMapCoord;

out vec4 fragColor;

uniform vec4 mainColor;
uniform float specularExponent;
uniform vec4 specularColor;
uniform sampler2D mainTexture;

#pragma include "assets/shaders/light.glsl"
#pragma include "assets/shaders/shadowmap.glsl"

void main(void)
{
    vec3 normal = normalize(vNormal);
    vec3 diffuse;
    float specular;
    getDirectionalLight(normal, _dLight, specularExponent, diffuse, specular);
    vec3 diffusePoint;
    float specularPoint;
    getPointLight(normal,vEcPosition, _pLights, specularExponent, diffusePoint, specularPoint);
    float visibility;
    if (SHADOWS){
        visibility = computeLightVisibility(vShadowMapCoord);
    } else {
        visibility = 1.0;
    }
    vec3 color = max((diffuse+diffusePoint)*visibility,_ambient.xyz)*mainColor.xyz;

    fragColor = vec4(texture(mainTexture,vUv).xyz*color.xyz, 1.0)+vec4((specular+specularPoint)*specularColor.xyz,0.0);
}
 