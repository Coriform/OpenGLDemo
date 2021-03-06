#version 330 core

// CLEAN THIS SHADER UP!


layout(location = 0) out vec4 outColor;

in vec2 Texcoord;
uniform sampler2D tSampleA;
uniform sampler2D tSampleB;

uniform int amount = 1;
uniform bool mult = true;

void main() 
{
	vec4 colorA = texture(tSampleA, Texcoord);
	vec4 colorB = texture(tSampleB, Texcoord);

	//if( colorA.a == 0 )
	//	colorA = vec4(0,0,0,0);

	//outColor = mix(colorA, colorB, ratio);

	outColor = vec4(0,0,0,1);

	for( int i = 0; i < amount; ++i )
	{
		if( mult )
			outColor += colorA * colorB;
		else
			outColor += colorA + colorB;
	}
	
}