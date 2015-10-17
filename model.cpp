#include "model.h"

namespace SE_CORE
{
	model::model(char *path)
	{
		_mesh.load(path);
	}

	void model::draw()
	{
		_mesh.draw();
	}
}