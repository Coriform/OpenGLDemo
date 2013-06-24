#version 150

uniform sampler2D tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
in vec3 Color;
in vec3 EyePos;
in vec3 LightPos;


out vec4 outColor;

vec3 light_direction = vec3(0.408248, -0.816497, 0.408248);
vec4 light_diffuse = vec4(0.8, 0.8, 0.8, 0.0);
vec4 light_ambient = vec4(0.2, 0.2, 0.2, 1.0);
vec4 light_specular = vec4(0.5, 0.5, 0.5, 1.0);

void main()
{
	vec4 texColor = texture( tex, Texcoord );
	//outColor = texColor;

	vec4 Id = light_diffuse;
	vec4 Is = light_specular;

	float n = 80;

	vec3 L = normalize(LightPos-Position);
	vec3 N = normalize(Normal);
	vec3 V = normalize(EyePos);

	vec3 Ri = (2 * dot(L,N) * N) - L;

	light_ambient = clamp(light_ambient, 0.0, 1.0);

	vec4 KdDot = Id * texColor * dot(N,L);
	KdDot = clamp(KdDot, 0.0, 1.0);

	vec4 KsDot = Is * pow(clamp(dot(V,Ri), 0.0, 1.0), n);
	KsDot = clamp(KsDot, 0.0, 1.0);

	outColor = light_ambient + KdDot + KsDot;

	/*
	vec3 light_dir = (view * model * vec4(light_direction, 0.0)).xyz;
	vec3 N = normalize(Normal);
	vec3 E = normalize(Position);
	vec3 R = reflect(light_dir, N);

	vec4 tex_diffuse = texture2D( tex, Texcoord );
	vec4 diffuse = max(-dot(N, light_dir), 0.0) * light_diffuse;
	vec4 ambient = diffuse + light_ambient;
	vec4 specular = max(pow(-dot(R, E), n), 0.0) * light_specular;

	outColor = specular * vec4(0.2,0.2,0.2,0.2) + ambient + tex_diffuse;
	*/
}


