#include "CommonLibs.h"
#include "Behavior.h"

namespace Roivas
{
	Behavior::Behavior() : Component(CT_Behavior)
	{

	}

	Behavior::Behavior(const Behavior& b) : Component(CT_Behavior)
	{

	}

	Behavior::~Behavior()
	{
		//GetSystem(Graphics)->RemoveComponent(this);
	}

	Behavior* Behavior::Clone()
	{
		return new Behavior(*this);
	}

	void Behavior::Initialize()
	{
		//GetSystem(Graphics)->AddComponent(this);
	}

	void Behavior::Deserialize(FileIO& fio, Json::Value& root)
	{

	}
}