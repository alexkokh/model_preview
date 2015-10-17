#include "model.h"

namespace SE_CORE
{
	model::model(char *path)
	{
		_RenderTarget.load(path);
	}

	void model::draw()
	{
		_RenderTarget.draw();
	}
}