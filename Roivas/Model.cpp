#include "CommonLibs.h"
#include "Model.h"

namespace Roivas
{
	Model::Model() : 
		MeshName(""), 
		DiffuseName(""), 
		NormalName(""),
		VertexBuffer(0),
		UVBuffer(0),
		NormalBuffer(0),
		ElementBuffer(0),
		Indices(0),
		DiffuseID(0), 
		NormalID(0),
		Color(vec4()), 
		WireColor(vec3(1,1,1)), 
		Component(CT_Model)
	{

	}

	Model::Model(const Model& m) : 
		MeshName(m.MeshName), 
		DiffuseName(m.DiffuseName), 
		NormalName(m.NormalName),
		VertexBuffer(m.VertexBuffer),
		UVBuffer(m.UVBuffer),
		NormalBuffer(m.NormalBuffer),
		ElementBuffer(m.ElementBuffer),
		Indices(m.Indices),
		DiffuseID(m.DiffuseID), 
		NormalID(m.NormalID),
		Color(m.Color), 
		WireColor(m.WireColor),
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
		fio.Read(root["NormalTexture"], NormalName);
		fio.Read(root["Color"], Color);
		fio.Read(root["WireColor"], WireColor);

		//MeshID		= GetSystem(Graphics)->LoadMesh(MeshName);
		GetSystem(Graphics)->LoadMesh(MeshName, VertexBuffer, UVBuffer, NormalBuffer, ElementBuffer, Indices);
		DiffuseID	= GetSystem(Graphics)->LoadTexture(DiffuseName); 
		NormalID	= GetSystem(Graphics)->LoadTexture(NormalName); 
	}
}