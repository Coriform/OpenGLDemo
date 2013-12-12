
#version 330 core

// Ouput data
layout(location = 0) out vec4 outColor;

in vec2 UV;

uniform sampler2D tLighting;
uniform sampler2D tEpipolar;


uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec2 screen_dims;

uniform mat4 V, P;


float epi_slices = 8;		// * 4 = 32
float epsilon = 0.0001;





vec4 PostProcessGodRays( vec2 OriginalUV )
{
	vec4 clip_light		= P * V * vec4(light_pos,1);
	vec3 ndc_light		= clip_light.xyz /= clip_light.w;
	vec2 screen_light	= (ndc_light.xy + 1.0) / 2.0;


	float Exposure = 0.45;
	float Decay = 0.96;
	float Density = 0.65;
	float Weight = 0.75;


    vec2 deltaTexCoord = (OriginalUV - screen_light.xy);

      
	float NUM_SAMPLES = 64.0;
    deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;  
   
    vec4 color = texture2D(tEpipolar, OriginalUV);  
     
    float illuminationDecay = 1.0f;  
    
    for (int i = 0; i < NUM_SAMPLES; i++)  
	{  
		OriginalUV -= deltaTexCoord;  
		vec4 sample = texture2D(tEpipolar, OriginalUV);  
		sample *= illuminationDecay * Weight;   
		color += sample;  
		illuminationDecay *= Decay;  
	}  
	 
	return vec4( color * Exposure);  
}



bool isBetween(vec2 a, vec2 b, vec2 c)
{
    float crossproduct = (c.y - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y);

	if( abs(crossproduct) > epsilon )
		return false;

	float dotproduct = (c.x - a.x) * (b.x - a.x) + (c.y - a.y) * (b.y - a.y);

	if( dotproduct < 0 )
		return false;

    float squaredlengthba = (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y);
   
   if( dotproduct > squaredlengthba)
		return false;

	return true;
}



void main( void )
{
	outColor = PostProcessGodRays(UV);
}







