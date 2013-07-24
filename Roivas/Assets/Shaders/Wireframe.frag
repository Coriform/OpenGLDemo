#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 Color;

void main()
{
	outColor = vec4( Color, 1.0 );
}