#version 330 core

const float Near = 0.1; // camera z near
const float Far = 300.0; // camera z far
const float LinearDepthConstant = 1.0 / (56.0);

in vec4 Position;

void main()
{
	float linearDepth = length(Position) * LinearDepthConstant;
	gl_FragColor.r = linearDepth;
}