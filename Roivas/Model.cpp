#include "CommonLibs.h"
#include "Model.h"

namespace Roivas
{
	Model::Model() : MeshName(""), DiffuseName(""), MeshID(0), DiffuseID(0), Color(vec4()), Component(CT_Model)
	{

	}

	Model::Model(const Model& m) : 
		MeshName(m.MeshName), 
		DiffuseName(m.DiffuseName), 
		MeshID(m.MeshID), 
		DiffuseID(m.DiffuseID), 
		Color(m.Color), 
		Component(CT_Model)
	{

	}

	Model::~Model()
	{
		GetSystem(Graphics)->RemoveComponent(this);
	}

	Model* Model::Clone()
	{
		return new Model(*this);
	}

	void Model::Initialize()
	{
		GetSystem(Graphics)->AddComponent(this);
	}

	void Model::Deserialize(FileIO& fio, Json::Value& root)
	{
		fio.Read(root["Mesh"], MeshName);
		fio.Read(root["DiffuseTexture"], DiffuseName);
		fio.Read(root["Color"], Color);

		MeshID		= GetSystem(Graphics)->LoadMesh(MeshName);
		DiffuseID	= GetSystem(Graphics)->LoadTexture(DiffuseName); 
	}
}