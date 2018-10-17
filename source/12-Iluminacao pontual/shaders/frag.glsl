#version 330 core

in vec3 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 ambientLightColor;
uniform float ambientLightStrength;

uniform vec3 lightPosition;
uniform vec3 lightColor;

out vec4 color;

void main()
{
	vec3 ambientLight = ambientLightStrength * ambientLightColor;

	vec3 normal = normalize(fragNormal);
	vec3 lightDirection = normalize(lightPosition - fragPosition);

	float diffuse = max(dot(normal, lightDirection), 0.0);
	vec3 diffuseLight = diffuse * lightColor * 0.5;

	vec3 iluminatedFragColor = (ambientLight + diffuseLight) * fragColor;

	color = vec4(iluminatedFragColor, 1.0f);
}