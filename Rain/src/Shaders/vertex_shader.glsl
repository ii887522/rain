#version 460

layout (binding = 0) uniform Eye
{
	mat4 projection;
	mat4 view;
} eye;

layout (location = 0) in vec2 position;
layout (location = 1) in mat4 model;

void main()
{
	gl_Position = eye.projection * eye.view * model * vec4(position, 0.0f, 1.0f);
}