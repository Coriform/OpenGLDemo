#version 330 core

// Ouput data
layout(location = 0) out vec4 outColor;

uniform sampler2D tDiffuse; 
uniform sampler2D tPosition;
uniform sampler2D tNormals;
uniform sampler2D tShadow;
uniform sampler2D tSpecular;

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

uniform float shadowsmooth = 16.0f;

const float shininess = 30.0;

const float bias = 0.001;

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

	vec3 LightDirection = ( V * vec4(lightdir,0) ).xyz;
	vec3 EyeDirection = vec3(0,0,0) - (V * position).xyz;
	vec4 ShadowCoord = Bias * ( DepthProj * DepthView * position );

	vec3 L  = normalize( LightDirection);
	vec3 E  = normalize( EyeDirection );
	vec3 N  = normalize( normal.xyz );		

	if( lighttype != 0 )
		L = normalize( (V * vec4(lightpos - position.xyz,0)).xyz );

	float d = length( position.xyz - lightpos );				
	float att = attenuation(lightradius, d);		

	vec3 R  = reflect(-L,N);

	vec3 Diffuse	= clamp( max( dot( N, L ), 0.0 ), 0.0, 1.0 ) * lightcolor * diffuse.xyz;            
	vec3 Specular	= clamp( texture( tSpecular, UV ).xyz * pow( max( dot( R, E ), 0.0 ), shininess ), 0.0, 1.0 ) * lightcolor;


	// Shadows
	float visibility = 1.0;
	vec3 ShadowH = ShadowCoord.xyz / ShadowCoord.w;
	//float shadow_sam = texture2D( tShadow, ShadowH.xy ).z;	

	
	for( int i = 0; i < shadowsmooth; ++i )
	{
		float shadow_sam = texture2D( tShadow, ShadowH.xy + poissonDisk[i]/1000.0 ).z;
		if( ShadowH.z > shadow_sam-bias )
			visibility -= 1.0f/shadowsmooth;
	}


	if( lighttype == 1 )
	{
		float lambertTerm = max( dot(N,L), 0.0);

        if( lambertTerm > 0.0 )
        {
            float cutoff = 0.99;
			float falloff = cutoff - clamp( lightcone, 0.0, 1.0 );

			float angle = dot(L, normalize(LightDirection));

			float outer = falloff;
			float inner = cutoff;

			float angle_diff = inner - outer;    

			float spot = clamp((angle - outer) /  angle_diff, 0.0, 1.0);
            color += (visibility * Diffuse * lambertTerm * spot * att);
            color += (visibility * Specular * spot * att);
        }
	}
	else if( lighttype == 2 )
	{
		color += ((visibility * Diffuse + visibility * Specular) * att);
	}
	else
	{
		color += (visibility * Diffuse + visibility * Specular);
	}

	outColor = vec4(color,1);
}