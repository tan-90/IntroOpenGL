#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 normal;

uniform mat4 transformation;

uniform mat4 model;

out vec3 fragColor;
out vec2 fragUv;
out vec3 fragNormal;
out vec3 fragPosition;
void main()
{
	gl_Position = transformation * vec4(position.x, position.y, position.z, 1.0);

	fragColor = color;
	fragUv = uv;
	fragNormal = normal;
	fragPosition = vec3(model * vec4(position, 1.0f));
}