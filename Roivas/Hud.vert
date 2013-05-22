#version 150

in vec2 position;

out vec3 Color;

uniform vec3 color;
uniform vec2 offset;


void main() 
{
	Color = color;
	gl_Position = vec4( position.x + offset.x, position.y + offset.y, 0, 1.0 );
}