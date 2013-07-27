#include "CommonLibs.h"
#include "Light.h"

namespace Roivas
{
	Light::Light() : 
		Color(vec3(1.0f,1.0f,1.0f)), 
		Direction(vec3(0.0f,-1.0f,0.0f)), 
		Radius(10.0f), 
		Cone(0.1f),
		ShadowMap(0),
		LightType("DirectionLight"),
		Type(LT_DirectionLight), 
		Component(CT_Light)
	{

	}

	Light::Light(const Light& l) : 
		Color(l.Color), 
		Direction(l.Direction), 
		Radius(l.Radius), 
		Cone(l.Cone),
		ShadowMap(0),
		LightType(l.LightType),
		Type(l.Type), 
		Component(CT_Light)
	{

	}

	Light::~Light()
	{
		GetSystem(Graphics)->RemoveComponent(this);
	}

	Light* Light::Clone()
	{
		return new Light(*this);
	}

	void Light::Initialize()
	{
		GetSystem(Graphics)->AddComponent(this);
	}

	void Light::Deserialize(FileIO& fio, Json::Value& root)
	{
		fio.Read(root["Color"], Color);
		fio.Read(root["Direction"], Direction);
		fio.Read(root["Radius"], Radius);	
		fio.Read(root["Cone"], Cone);

		Direction = glm::normalize(Direction);

		fio.Read(root["LightType"], LightType);

		if( LightType == "SpotLight" )
			Type = LT_SpotLight;
		else if( LightType == "PointLight" )
			Type = LT_PointLight;
		else if( LightType == "CapsuleLight" )
			Type = LT_CapsuleLight;
		else
			Type = LT_DirectionLight;		
	}
}