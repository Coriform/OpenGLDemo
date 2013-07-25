#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position;
in vec3 Normal;
in vec3 EyeDirection;
in vec3 LightDirection;
in vec4 ShadowCoord;

// Ouput data
layout(location = 0) out vec4 outColor;

uniform sampler2D tex_sampler;
uniform sampler2DShadow shadow_sampler;


const vec3 ambient_light = vec3(0.2,0.2,0.2);
const vec3 model_specular = vec3(1,1,1);
const float shininess = 70.0;
const float radius = 10.0;


void main()
{
	vec4 texColor = texture( tex_sampler, UV );

	vec3 N = normalize( Normal );
	vec3 L = normalize( LightDirection);
	vec3 E = normalize( EyeDirection );
	vec3 R = reflect(-L,N);

	vec3 Ambient	= clamp( texColor.xyz * ambient_light, 0.0, 1.0 );	
	vec3 Diffuse	= clamp( max( dot( N, L ), 0.0 ), 0.0, 1.0 ) * vec3(1,1,1) * texColor.xyz;              
	vec3 Specular	= clamp( pow( max( dot( R, E ), 0.0 ), shininess ), 0.0, 1.0 ) * model_specular;

	vec3 color = Ambient + Diffuse + Specular;


//	// Shadows
	float bias = 0.005;

	float light_depth = texture( shadow_sampler, vec3(ShadowCoord.xy,  (ShadowCoord.z-bias)/ShadowCoord.w) ) + bias;
	float pixel_depth = ShadowCoord.z;	

	float xw = ShadowCoord.x/ShadowCoord.w;
	float yw = ShadowCoord.y/ShadowCoord.w;

	if( ShadowCoord.w > bias && xw > bias && xw < 1-bias && yw > 0 && yw < 1-bias ) 
	{
		if( pixel_depth > light_depth )
			color = Ambient;
	}
//

	outColor = vec4(color,1);
	//outColor = vec4(gl_FragCoord.z/2, gl_FragCoord.z/2, gl_FragCoord.z/2, gl_FragCoord.z/2 );
}