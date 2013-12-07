

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
	SH_Ambient,
	SH_Deferred,
	SH_Screen,
	SH_Hud,
	SH_Wireframe,
	SH_ShadowTex,
	SH_Lighting,
	SH_LightingSSM,
	SH_LightingESM,
	SH_LogBlur,
	SH_Fog,

	SH_TOTAL
};


// RenderTargets
enum RenderTarget
{
	RT_LightDepth = 0,
	RT_LightDiffuse,
	RT_LightPositions,
	RT_LightNormals,

	RT_SceneLighting,

	RT_SceneDiffuse,
	RT_ScenePositions,
	RT_SceneNormals,	
	RT_SceneSpecular,
	RT_SceneDepth,
	

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


