in vec2 vUv;
in vec3 vNormal;
in vec3 vEcPosition;

out vec4 fragColor;

uniform vec4 mainColor;
uniform float specularExponent;
uniform vec4 specularColor;
uniform sampler2D mainTexture;

#pragma include "assets/shaders/light.glsl"

void main(void)
{
    vec3 normal = normalize(vNormal);
    vec3 diffuse;
    float specular;
    getDirectionalLight(normal, _dLight, specularExponent, diffuse, specular);
    vec3 diffusePoint;
    float specularPoint;
    getPointLight(normal,vEcPosition, _pLights,specularExponent,diffusePoint,specularPoint);
    vec4 color = vec4(max(diffuse+diffusePoint,_ambient.xyz),1.0)*mainColor;

    fragColor = texture(mainTexture,vUv)*color+vec4((specular+specularPoint)*specularColor.xyz,0.0);
}
 