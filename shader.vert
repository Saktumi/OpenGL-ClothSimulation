#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 viewProj;
uniform mat4 model;

out vec3 fragNormal;


void main()
{
    gl_Position = viewProj * model * vec4(position, 1.0);

	fragNormal = vec3(model * vec4(normal, 0));
}