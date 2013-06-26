#pragma once
#include <cmath>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
typedef glm::quat quat;

#define MatToArray(name) glm::value_ptr( (name) )		// Added because "value_ptr" is a confusing name

const float PI = 3.14159f;
const float EPSILON = 0.0001f;


inline void Normalize( vec3& x )
{
	float sqr = x.x * x.x + x.y * x.y + x.z * x.z;

	if( sqr < -EPSILON || sqr > EPSILON )
		x = x * glm::inversesqrt(sqr);
}