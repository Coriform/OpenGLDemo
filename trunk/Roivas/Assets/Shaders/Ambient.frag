
#version 330 core

// Ouput data
layout(location = 0) out vec4 outColor;

uniform sampler2D tDiffuse; 

// Interpolated values from the vertex shaders
in vec2 UV;

const vec3 ambient_light = vec3(0.1,0.1,0.1);

void main( void )
{
	vec4 diffuse = texture( tDiffuse, UV );
	outColor = vec4( clamp( diffuse.xyz * ambient_light, 0.0, 1.0 ), 1);
}







