
#version 330 core

// Ouput data
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outCoord;

in vec2 UV;

uniform sampler2D tLighting;
uniform sampler2D tDepth;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec2 screen_dims;

uniform mat4 V, P;


float epi_slices = 8;		// * 4 = 32
float epsilon = 0.0001;



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
	outColor = vec4(0,0,0,1);//texture2D( tLighting, UV);
	outColor = texture2D( tDepth, UV );

	float depth = texture2D( tDepth, UV ).z;

	outCoord = vec4(0,1,0,1);


	vec4 clip_light		= P * V * vec4(light_pos,1);
	vec3 ndc_light		= clip_light.xyz /= clip_light.w;
	vec2 screen_light	= (ndc_light.xy + 1.0) / 2.0;

	float w = 1.0/screen_dims.x;
	float h = 1.0/screen_dims.y;


	
	for( float i = 0; i < epi_slices; i += 1.0 )
	{
		float step = i/epi_slices;

		//if( UV.x >= step && UV.x <= step+w && (UV.y <= 0.0+(5*h) || UV.y >= 1.0-(5*h)) )
		//	outColor += vec4(1,0,0,1);

		//step = (i+1)/epi_slices;

		//if( UV.y >= step && UV.y <= step+h && (UV.x <= 0.0+(5*w) || UV.x >= 1.0-(5*w)) )
		//	outColor += vec4(0,1,0,1);
		

		if( isBetween( screen_light, vec2(1,step), UV ) )
			outColor += vec4(1,1,0,1);	//Yellow Bottom

		if( isBetween( screen_light, vec2(step,0), UV ) )
			outColor += vec4(1,0,0,1);	//Red Bottom

		step = (i+1)/epi_slices;

		if( isBetween( screen_light, vec2(step,1), UV ) )
			outColor += vec4(0,1,0,1);	//Green Top		

		if( isBetween( screen_light, vec2(0,step), UV ) )
			outColor += vec4(0,0,1,1);	//Blue Left

	}


	//outColor = vec4(screen_light,1);
	

	/*
	if( UV.x >= 0.5 && UV.x <= 0.5+(1/screen_dims.x))
	{
		outColor = vec4(1,0,0,1);
	}
	else
	{
		outColor = vec4(0,0,0,1);
	}
	*/





	// Occlusion
	vec4 c = texture2D( tLighting, UV );

	outColor = c;
	
}







