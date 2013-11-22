#version 330 core

// Ouput data
layout(location = 0) out vec4 outColor;


void main()
{
	outColor = gl_FragCoord;
}