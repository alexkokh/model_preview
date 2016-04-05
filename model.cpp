#include "model.h"

namespace SE_CORE
{
	model::model(string name, ShaderInfo *shaders, uint32_t numShaders, char *path) : mesh(name, shaders, numShaders)
	{
		load(path);
	}

	model::~model()
	{
	}
}