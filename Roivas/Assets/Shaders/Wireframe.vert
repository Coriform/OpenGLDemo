#version 150

in vec3 position;
out vec3 Position;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 wirecolor;

void main()
{
	Color = wirecolor;
	Position = position;
	gl_Position = proj * view * model * vec4( position, 1.0 );
}