#version 150

uniform sampler2D tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
in vec3 Color;

uniform sampler2D shadowtex;
in vec4 Shadowcoord;

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




	///*
       
	vec3 N = normalize( Normal );	
	vec3 L = normalize( vec3(0.5f,2,2) );
	//vec3 L = normalize( lightpos[1] - Position );
	vec3 E = normalize( eyepos - Position );
	vec3 R = -reflect( L, N );	

	vec3 ambient	= clamp( texColor.xyz * ambient_light, 0.0, 1.0 );	
	vec3 diffuse	= clamp( max( dot( N, L ), 0.0 ), 0.0, 1.0 ) * vec3(1,1,1) * texColor.xyz;              
	vec3 specular	= clamp( pow( max( dot( R, E ), 0.0 ), shininess ), 0.0, 1.0 ) * model_specular;



	float EPSILON = 0.005;    

	float light_depth = texture2DProj(shadowtex,  Shadowcoord).w + EPSILON;
	float pixel_depth = Shadowcoord.w;

	//vec4 color = PhongLighting();
	//vec4 shadow = AmbientLighting();

	//if( pixel_depth > 4 )
	//shadow = PhongLighting() - (1.0/pixel_depth*2);

	float xw = Shadowcoord.x/Shadowcoord.w;
	float yw = Shadowcoord.y/Shadowcoord.w;

	vec4 color = vec4( ( ambient + diffuse + specular ), 1.0 );

	if( Shadowcoord.w > EPSILON && xw > EPSILON && xw < 1-EPSILON && yw > 0 && yw < 1-EPSILON ) {

		if( pixel_depth > light_depth )
			color = vec4(ambient,1.0);
	}
       
	//outColor		+= vec4( ( ambient + diffuse + specular ) * att, 1.0); 
	outColor = color;
	//*/


	/*
	for( int i = 1; i < 2; ++i )
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



		float EPSILON = 0.005;    

		float light_depth = texture2DProj(shadowtex,  Shadowcoord).w + EPSILON;
		float pixel_depth = Shadowcoord.w;

		//vec4 color = PhongLighting();
		//vec4 shadow = AmbientLighting();

		//if( pixel_depth > 4 )
		//shadow = PhongLighting() - (1.0/pixel_depth*2);

		float xw = Shadowcoord.x/Shadowcoord.w;
		float yw = Shadowcoord.y/Shadowcoord.w;

		//vec4 color = vec4( ( ambient + diffuse + specular ) * att, 1.0);
		vec4 color = vec4( ( ambient + diffuse + specular ), 1.0 );

		if( Shadowcoord.w > EPSILON && xw > EPSILON && xw < 1-EPSILON && yw > 0 && yw < 1-EPSILON ) {

			if( pixel_depth > light_depth )
				color = vec4(ambient,1.0);
		}
       
		//outColor		+= vec4( ( ambient + diffuse + specular ) * att, 1.0); 
		outColor += color;
	}
	*/

}


