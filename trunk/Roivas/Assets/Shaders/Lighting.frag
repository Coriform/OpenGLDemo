#version 330 core

// Ouput data
layout(location = 0) out vec4 outColor;

uniform sampler2D tDiffuse; 
uniform sampler2D tPosition;
uniform sampler2D tNormals;
uniform sampler2D tSpecular;

uniform mat4 V;

uniform vec2 screensize;
uniform vec3 lightpos;
uniform vec3 lightcolor;
uniform vec3 lightdir;
uniform float lightradius;
uniform float lightcone;
uniform int lighttype;

const vec3 ambient_light = vec3(0.1,0.1,0.1);
const vec3 model_specular = vec3(1,1,1);
const float shininess = 30.0;

const float bias = 0.001;


float attenuation(float r, float d)
{
    return max(0.0, 1.0 - (d / r));
}


void main()
{
	vec2 UV = gl_FragCoord.xy/ screensize;

	vec4 diffuse	= texture( tDiffuse, UV );
	vec4 position	= texture( tPosition, UV );
	vec4 normal		= texture( tNormals, UV );

	vec3 Ambient = vec3(0,0,0);//clamp( diffuse.xyz * ambient_light, 0.0, 1.0 );	
	vec3 color = Ambient;

	vec3 LightDirection = ( V * vec4(lightdir,0) ).xyz;
	vec3 EyeDirection = vec3(0,0,0) - (V * position).xyz;

	vec3 L  = normalize( LightDirection);
	vec3 E  = normalize( EyeDirection );
	vec3 N  = normalize( normal.xyz );	

	if( lighttype != 0 )
		L = normalize( (V * vec4(lightpos - position.xyz,0)).xyz );



	float d = length( position.xyz - lightpos );				
	float att = attenuation(lightradius, d);

	vec3 R  = reflect(-L,N);
	
	vec3 Diffuse	= clamp( max( dot( N, L ), 0.0 ), 0.0, 1.0 ) * lightcolor * diffuse.xyz;              
	vec3 Specular	= clamp( texture( tSpecular, UV ).xyz * pow( max( dot( R, E ), 0.0 ), shininess ), 0.0, 1.0 ) * model_specular;

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
            color += (Diffuse * lambertTerm * spot * att);
            color += (Specular * spot * att);
        }
	}
	else if( lighttype == 2 )
	{
		color += ((Diffuse + Specular) * att);
	}
	else
	{
		color += (Diffuse + Specular);
	}

	color = max( color, Ambient );

	outColor = vec4(color,1);
}