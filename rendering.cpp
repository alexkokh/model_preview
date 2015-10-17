#include "rendering.h"

namespace SE_CORE
{
	render_target::render_target()
	{
		_initialized = init_gl_buffers();
	}

	render_target::~render_target()
	{
		glDeleteVertexArrays(1, &_VertexArrayID);
		glDeleteBuffers(NumBuffers, _BufferIDs);
	}

	int render_target::init_gl_buffers()
	{
		glGenVertexArrays(1, &_VertexArrayID);
		if (!_VertexArrayID)
			return glGetError();

		glGenBuffers(NumBuffers, _BufferIDs);
		for (uint32_t i = 0; i < NumBuffers; i++)
		{
			if (!_BufferIDs[i])
				return glGetError();
		}

		return 1;
	}
}