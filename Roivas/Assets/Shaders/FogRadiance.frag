#version 330 core

// CLEAN THIS SHADER UP!


layout(location = 0) out vec4 outAttenuatedRadiance;
layout(location = 1) out vec4 outScatteredRadiance;

in vec2 Texcoord;
uniform sampler2D tInRadiance;
uniform sampler2D tDepth;

uniform float sigmaA = 0.2;
uniform float sigmaS = 0.8;

const float e = 2.71828;

void main() 
{
	vec4 color = texture(tInRadiance, Texcoord);
	float depth = texture(tDepth, Texcoord).z;

	float sigmaT = sigmaA + sigmaS;	

	outAttenuatedRadiance = color * pow(10,-depth*sigmaT);
	outScatteredRadiance = color * pow(10,-depth*sigmaA) * (1 - pow(10,-depth*sigmaS));
}