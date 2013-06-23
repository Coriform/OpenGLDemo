#include "CommonLibs.h"
#include "Transform.h"

namespace Roivas
{
	Transform::Transform() : Position(vec3()), Scale(vec3()), Component(CT_Transform)
	{

	}

	Transform::Transform(const Transform& t) : Position(t.Position), Scale(t.Scale), Component(CT_Transform)
	{

	}

	Transform* Transform::Clone()
	{
		return new Transform(*this);
	}

	void Transform::Initialize()
	{

	}

	void Transform::Deserialize(FileIO& fio, Json::Value& root)
	{
		fio.Read(root["Position"], Position);
		fio.Read(root["Scale"], Scale);
	}
}