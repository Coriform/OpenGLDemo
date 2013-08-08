#version 330 core

// Ouput data
layout(location = 0) out float depth;


void main()
{
	depth = gl_FragCoord.z;
}