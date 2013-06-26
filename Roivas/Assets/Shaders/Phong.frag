#version 150

uniform sampler2D tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
in vec3 Color;

uniform vec3 eyepos;
uniform vec3 overrideColor;

uniform vec3 lightpos[2];
uniform vec3 lightcolor[2];
uniform float lightradius[2];

out vec4 outColor;

const vec3 ambient_light = vec3(0.2,0.2,0.2);
const vec3 model_specular = vec3(1,1,1);
const float shininess = 70.0;
const float radius = 10.0;

void main()
{
	vec4 texColor = texture( tex, Texcoord );
	outColor = vec4(0,0,0,0);

	for( int i = 0; i < 2; ++i )
	{
		float dist		= length( Position -  lightpos[i] );				
		float att		= 1.0 - pow(dist/lightradius[i], 2); 
       
		vec3 N = normalize( Normal );	
		vec3 L = normalize( lightpos[i] - Position );
		vec3 E = normalize( eyepos - Position );
		vec3 R = -reflect( L, N );	

		vec3 ambient	= clamp( texColor.xyz * ambient_light, 0.0, 1.0 );	
		vec3 diffuse	= clamp( max( dot( N, L ), 0.0 ), 0.0, 1.0 ) * lightcolor[i] * texColor.xyz;              
		vec3 specular	= clamp( pow( max( dot( R, E ), 0.0 ), shininess ), 0.0, 1.0 ) * model_specular;
       
		outColor		+= vec4( ( ambient + diffuse + specular ) * att, 1.0); 
	}
}


