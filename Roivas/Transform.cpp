#include "CommonLibs.h"
#include "Transform.h"

namespace Roivas
{
	Transform::Transform() : Position(vec3()), Scale(vec3()), Rotation(vec3()), Component(CT_Transform)
	{

	}

	Transform::Transform(const Transform& t) : Position(t.Position), Scale(t.Scale), Rotation(t.Rotation), Component(CT_Transform)
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
		fio.Read(root["Rotation"], Rotation);
	}
}