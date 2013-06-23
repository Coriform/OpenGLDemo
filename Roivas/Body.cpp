#include "CommonLibs.h"
#include "Body.h"

namespace Roivas
{
	Body::Body() : Component(CT_Body)
	{

	}

	Body::Body(const Body& b) : Component(CT_Body)
	{

	}

	void Body::Initialize()
	{

	}

	Body* Body::Clone()
	{
		return new Body(*this);
	}
}