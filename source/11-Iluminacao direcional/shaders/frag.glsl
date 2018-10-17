#version 330 core

in vec3 fragColor;
in vec3 fragNormal;

uniform vec3 ambientLightColor;
uniform float ambientLightStrength;

uniform vec3 lightDirection;
uniform vec3 lightColor;

out vec4 color;

void main()
{
	vec3 ambientLight = ambientLightStrength * ambientLightColor;

	float diffuseStrength = max(dot(fragNormal, lightDirection), 0);
	vec3 diffuseLight = diffuseStrength * lightColor;

	vec3 iluminatedFragColor = (ambientLight + diffuseLight) * fragColor;

	color = vec4(iluminatedFragColor, 1.0f);
}