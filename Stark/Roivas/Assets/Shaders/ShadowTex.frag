#version 330 core

in vec4 Position;

void main()
{
	gl_FragColor.r = gl_FragCoord.z;
}