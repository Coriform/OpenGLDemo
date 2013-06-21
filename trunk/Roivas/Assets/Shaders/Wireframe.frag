#version 150

in vec3 Position;
in vec3 Color;

out vec4 outColor;

void main()
{
	outColor = vec4( Color, 1.0 );
}