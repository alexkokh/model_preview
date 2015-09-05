#include "model.h"

namespace SE_CORE
{
	model::model(char *path)
	{
		m_RenderTarget.load(path);
	}

	void model::draw()
	{
		m_RenderTarget.draw();
	}
}