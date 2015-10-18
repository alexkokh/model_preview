#include "model.h"

namespace SE_CORE
{
	model::model(ShaderInfo *shaders, int numShaders, char *path) : _mesh(shaders, numShaders)
	{
		_mesh.load(path);
	}

	void model::draw()
	{
		_mesh.draw();
	}

	GLuint model::get_program()
	{
		return _mesh.get_program();
	}
}