
#version 330 core

// Ouput data
layout(location = 0) out vec4 outDiffuse;
layout(location = 1) out vec4 outPositions;
layout(location = 2) out vec4 outNormals;
layout(location = 3) out vec4 outSpec; 
layout(location = 4) out vec4 outDepth;

in vec2 UV;
in vec3 Position;
in vec3 EyeDirection;
in vec3 Normal;
in float Depth;

uniform sampler2D tex_sampler;
uniform sampler2D norm_sampler;
uniform sampler2D spec_sampler;

uniform bool normal_mapping = true;

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


void main( void )
{
	vec3 E  = normalize( EyeDirection );
	vec3 N  = normalize( Normal );	
	vec3 PN = perturb_normal(N, E, UV);	// Normal mapping

	if( normal_mapping == false )
		PN = N;

	outDiffuse		= texture( tex_sampler, UV );
	outPositions	= vec4( Position, 1 );
	outNormals		= vec4( PN, Depth );
	outDepth		= vec4(Depth)/100;
	outSpec			= texture( spec_sampler, UV );
}







