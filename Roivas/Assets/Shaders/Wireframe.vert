
#version 330 core

layout(location = 0) in vec3 position;

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 wirecolor;

void main()
{
	Color = wirecolor;
	gl_Position = proj * view * model * vec4( position, 1.0 );
}