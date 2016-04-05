#pragma once

#include "vmath.h"
#include "mesh.h"
#include "DAEReader.h"

namespace SE_CORE
{
	class model : public mesh
	{
	public:
		model(string name, ShaderInfo *shaders, uint32_t numShaders, char *path);
		virtual ~model();
	};
}