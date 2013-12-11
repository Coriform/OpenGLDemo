#version 330 core

// Ouput data
layout(location = 0) out vec4 outLight;
layout(location = 1) out vec4 outShadow;
layout(location = 2) out vec4 outBloom;

uniform sampler2D tDiffuse; 
uniform sampler2D tPosition;
uniform sampler2D tNormals;
uniform sampler2D tSpecular;
uniform sampler2D tShadow;

uniform mat4 V;

uniform mat4 DepthProj;
uniform mat4 DepthView;
uniform mat4 Bias;

uniform vec2 screensize;
uniform vec3 lightpos;
uniform vec3 lightcolor;
uniform vec3 lightdir;
uniform float lightradius;
uniform float lightcone;
uniform int lighttype;

uniform float shadowsmooth = 2.0f;

const float shininess = 30.0;

const float bias = 0.00001;

float attenuation(float r, float d)
{
    return max(0.0, 1.0 - (d / r));
}

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);


void main()
{
	vec2 UV = gl_FragCoord.xy/ screensize;

	vec4 diffuse	= texture( tDiffuse, UV );
	vec4 position	= texture( tPosition, UV );
	vec4 normal		= texture( tNormals, UV );

	vec3 color = vec3(0,0,0);
	vec3 bloomcolor = vec3(0,0,0);

	vec3 LightDirection = ( V * vec4(lightdir,0) ).xyz;
	vec3 EyeDirection = vec3(0,0,0) - (V * position).xyz;
	vec4 ShadowCoord = Bias * ( DepthProj * DepthView * position );

	vec3 L  = normalize( LightDirection);
	vec3 E  = normalize( EyeDirection );
	vec3 N  = normalize( normal.xyz );		

	if( lighttype != 0 )
		L = normalize( (vec4(lightpos - position.xyz,0)).xyz );

	float d = length( position.xyz - lightpos );				
	float att = attenuation(lightradius, d);	

	vec3 R  = reflect(-L,N);

	vec3 Diffuse	= clamp( max( dot( N, L ), 0.0 ), 0.0, 1.0 ) * lightcolor * diffuse.xyz;            
	vec3 Specular	= clamp( texture( tSpecular, UV ).xyz * pow( max( dot( R, E ), 0.0 ), shininess ), 0.0, 1.0 ) * lightcolor;


	// Shadows
	float visibility = 1.0;
	vec3 ShadowH = ShadowCoord.xyz / ShadowCoord.w;

	float shadow_sam = texture( tShadow, ShadowH.xy ).z;
	if( ShadowH.z > shadow_sam-bias )
		visibility -= 0.75f;
	
	vec3 shadowed_color = vec3(0,0,0);

	// Spot Light
	if( lighttype == 1 )
	{
		float cutoff = 0.99;
		float falloff = cutoff - clamp( lightcone, 0.0, 1.0 );

		float angle = dot(L, normalize(LightDirection));

		float outer = falloff;
		float inner = cutoff;

		float angle_diff = inner - outer;	

		float spot = clamp((angle - outer) /  angle_diff, 0.0, 1.0);

		float lambertTerm = max( dot(N,L), 0.0);
		if( lambertTerm > 0.0 )
		{
			color += Diffuse * lambertTerm * spot * att + Specular * spot * att;
			shadowed_color += (visibility * Diffuse * lambertTerm * spot * att);
			shadowed_color += (visibility * Specular * spot * att);
			bloomcolor += visibility * Specular * spot * att;
		}
	}
	// Point Light
	else if( lighttype == 2 )
	{
		visibility = 1.0;
		color += ((visibility * Diffuse + visibility * Specular) * att);
		shadowed_color = color;
		bloomcolor += visibility * Specular * att;
	}
	// Directional Light
	else
	{
		color += Diffuse + Specular;
		shadowed_color += visibility * Diffuse;
		bloomcolor += visibility * Specular;

		if( visibility < 1.0 )
		{
			color -= Specular;
			bloomcolor = vec3(0,0,0);
		}
	}

	outLight = vec4(color,1);
	outBloom = vec4(bloomcolor,1);

	if( visibility < 1.0 )
		outShadow = vec4(shadowed_color,1);
	else
		outShadow = vec4(1,1,1,0);
}