#include "CommonLibs.h"
#include "Light.h"

namespace Roivas
{
	Light::Light() : Color(vec3(1.0f,1.0f,1.0f)), Radius(10.0f), Component(CT_Light)
	{

	}

	Light::Light(const Light& l) : Color(l.Color), Radius(l.Radius), Component(CT_Light)
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
		fio.Read(root["Radius"], Radius);
	}
}