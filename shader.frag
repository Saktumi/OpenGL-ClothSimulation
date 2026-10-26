#version 330 core

in vec3 fragNormal;

uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection = normalize(vec3(1, 5, 2));
uniform vec3 LightColor = vec3(1);
uniform vec3 DiffuseColor;

out vec4 fragColor;

void main()
{

	vec3 irradiance = AmbientColor + LightColor * max(0, dot(LightDirection, normalize(fragNormal)));

	vec3 reflectance = irradiance * DiffuseColor;

	fragColor = vec4(sqrt(reflectance), 1);
}
