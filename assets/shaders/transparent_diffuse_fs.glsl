in vec2 vUv;
in vec3 vNormal;
in vec3 vEcPosition;

out vec4 fragColor;

uniform vec4 mainColor = vec4(1.0,1.0,1.0,1.0);
uniform float specularExponent;
uniform vec4 specularColor;
uniform sampler2D mainTexture;

#pragma include "assets/shaders/light.glsl"

void main(void)
{
    vec3 normal = normalize(vNormal);
    vec3 diffuseDirectionalLight = getDirectionalLightDiffuse(normal,_dLight);
    vec3 diffusePointLight = getPointLightDiffuse(normal,vEcPosition, _pLights);
    vec4 color = vec4(max(diffuseDirectionalLight+diffusePointLight,_ambient.xyz),1.0)*mainColor;

    fragColor = texture(mainTexture,vUv)*color;
}
 