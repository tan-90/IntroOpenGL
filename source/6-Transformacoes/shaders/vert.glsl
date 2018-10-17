#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 transformation;

out vec3 fragColor;
void main()
{
	gl_Position = transformation * vec4(position.x, position.y, position.z, 1.0);
	fragColor = color;
}