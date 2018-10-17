#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 fragColor;
in vec2 fragUv;
in vec3 fragNormal;
in vec3 fragPosition;

uniform Material material;
uniform Light light;

uniform vec3 cameraPosition;

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;

out vec4 color;

void main()
{
	// ambient
	vec3 ambientColor = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(fragNormal);
	vec3 lightDirection = normalize(light.position - fragPosition);
	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuseColor = light.diffuse * (diff * material.diffuse);

	// specular
	vec3 viewDir = normalize(cameraPosition - fragPosition);
	vec3 reflectedDir = reflect(-lightDirection, norm);
	float spec = pow(max(dot(viewDir, reflectedDir), 0.0), material.shininess);
	vec3 specularColor = light.specular * (spec * material.specular);

	vec3 iluminatedFragColor = ambientColor + diffuseColor + specularColor;
	color = vec4(iluminatedFragColor, 1.0f);
	//color = texture(textureSampler, fragUv) * vec4(iluminatedFragColor, 1.0f);
}