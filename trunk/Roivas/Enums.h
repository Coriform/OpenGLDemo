

// Systems
enum SystemType
{
	SYS_Window = 0,

	SYS_Graphics,
	SYS_Factory,
	SYS_Game,

	SYS_TOTAL
};


// Components
enum ComponentType
{
	CT_NONE = 0,

	CT_Behavior,
	CT_Body,
	CT_Light,
	CT_Model,
	CT_Player,
	CT_Transform,

	CT_TOTAL
};


// Shaders
enum ShaderName
{
	SH_Default = 0,
	SH_Deferred,
	SH_Screen,
	SH_Hud,
	SH_Wireframe,
	SH_ShadowTex,
	SH_Lighting,
	SH_LightingSSM,

	SH_TOTAL
};


// RenderTargets
enum RenderTarget
{
	RT_Lighting = 0,

	RT_Depth,
	RT_Diffuse,
	RT_Positions,
	RT_Normals,

	RT_TOTAL
};


// Types of lights
enum LightingType
{
	LT_DirectionLight,
	LT_SpotLight,
	LT_PointLight,
	LT_CapsuleLight,

	LT_TOTAL
};

