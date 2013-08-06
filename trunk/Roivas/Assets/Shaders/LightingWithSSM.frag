#version 330 core

// Ouput data
layout(location = 0) out vec4 outColor;

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position;
in vec3 Normal;
in vec3 EyeDirection;
in vec3 LightDirection;
in vec4 ShadowCoord;

uniform sampler2D tex_sampler;
uniform sampler2D norm_sampler;
uniform sampler2DShadow shadow_sampler;

uniform mat4 V;
uniform mat4 M;

uniform vec3 lightpos;
uniform vec3 lightcolor;
uniform vec3 lightdir;
uniform float lightradius;
uniform float lightcone;
uniform int lighttype;

uniform int num_lights = 0;

uniform bool normal_mapping = true;


const vec3 ambient_light = vec3(0.1,0.1,0.1);
const vec3 model_specular = vec3(1,1,1);
const float shininess = 30.0;

const float bias = 0.01;


// http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
	vec3 map = texture(norm_sampler, texcoord ).xyz;

	if( map.r < bias && map.g < bias && map.b < bias )
		return normalize(N);

	map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(TBN * map);
}

vec2 poissonDisk[4] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 )
);

void main()
{
	vec4 texColor = texture( tex_sampler, UV );

	vec3 Ambient = clamp( texColor.xyz * ambient_light, 0.0, 1.0 ) / num_lights;	
	vec3 color = Ambient;

	vec3 LightDirection = (V*vec4(lightdir,0)).xyz;

	vec3 L  = normalize( LightDirection);
	vec3 E  = normalize( EyeDirection );
	vec3 N  = normalize( Normal );	
	vec3 PN = perturb_normal(N, E, UV);	// Normal mapping

	if( normal_mapping == false )
		PN = N;

	if( lighttype != 0 )
		L = normalize( (V* vec4(lightpos - Position,0)).xyz );

	float dist = length( Position - lightpos );	
	float d = max(dist - lightradius, 0) / lightradius + 1.0;			
	float att = max( (1.0 / (d*d) - bias) / (1 - bias), 0 );		

	vec3 R  = reflect(-L,PN);

	vec3 Diffuse	= clamp( max( dot( N, L ), 0.0 ), 0.0, 1.0 ) * lightcolor * texColor.xyz;              
	vec3 Specular	= clamp( pow( max( dot( R, E ), 0.0 ), shininess ), 0.0, 1.0 ) * model_specular;

	// Shadows
	float visibility=1.0;

	for( int j = 0; j < 4; ++j )
	{
		//if ( texture( shadowMap, (ShadowCoord.xy/ShadowCoord.w) ).z  <  (ShadowCoord.z-bias)/ShadowCoord.w )
		float shadow_sam = texture( shadow_sampler, vec3(ShadowCoord.xy/ShadowCoord.w + poissonDisk[j]/1000.0,  (ShadowCoord.z-bias)/ShadowCoord.w) );
		visibility -= 0.3*(1.0-shadow_sam);
	}

	if( lighttype == 1 )
	{
		float cutoff = 0.99;
		float falloff = cutoff - clamp( lightcone, 0.0, 1.0 );

		float angle = dot(L, normalize(LightDirection));

		float outer = falloff;
		float inner = cutoff;

		float angle_diff = inner - outer;	

		float spot = clamp((angle - outer) /  angle_diff, 0.0, 1.0);

		float lambertTerm = max( dot(PN,L), 0.0);
		if( lambertTerm > 0.0 )
		{
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

	color = max( color, Ambient );

	outColor = vec4(color,1);
}