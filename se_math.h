#pragma once

#include "common.h"
#include "rendering.h"

namespace SE_CORE
{
	float mat4det(mat4 Xm);
	mat4 inverseMat4(mat4 Xm);
	bool rayTriangleIntersect(const vec3& org, const vec3& dir, const triangle_t& t);
	vec3 mat4MulVec3(const mat4& m, const vec3& v);
}