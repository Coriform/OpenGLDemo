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
uniform sampler2D norm_sampler;
uniform sampler2DShadow shadow_sampler;

uniform mat4 V;
uniform mat4 M;


const vec3 ambient_light = vec3(0.2,0.2,0.2);
const vec3 model_specular = vec3(1,1,1);
const float shininess = 30.0;
const float radius = 10.0;

const float bias = 0.005;


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
	map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame(N, -V, texcoord);
    return normalize(TBN * map);
}



void main()
{
	vec4 texColor = texture( tex_sampler, UV );
	//vec4 normColor = texture( norm_sampler, UV)*2.0-1.0;
	vec4 normColor = texture( norm_sampler, UV);

	vec3 L  = normalize( LightDirection);
	vec3 E  = normalize( EyeDirection );
	vec3 N  = normalize( Normal );
	vec3 PN = perturb_normal(N, E, UV);	
	vec3 R  = reflect(-L,PN);

	vec3 Ambient	= clamp( texColor.xyz * ambient_light, 0.0, 1.0 );		
	vec3 Diffuse	= clamp( max( dot( N, L ), 0.0 ), 0.0, 1.0 ) * vec3(1,1,1) * texColor.xyz;              
	vec3 Specular	= clamp( pow( max( dot( R, E ), 0.0 ), shininess ), 0.0, 1.0 ) * model_specular;

	vec3 color = Ambient + Diffuse + Specular;

	


//	// Shadows

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
}