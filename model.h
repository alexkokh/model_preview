#pragma once

#include "vmath.h"
#include "mesh.h"
#include "DAEReader.h"

namespace SE_CORE
{
	class model
	{
	private:
		vmath::vec3 _Position;
		vmath::vec3 _Rotation;
		vmath::vec3 _Scale;
		mesh _mesh;
	public:
		model(char *path);
		void draw();
	};
}