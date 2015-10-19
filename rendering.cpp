#include "rendering.h"

namespace SE_CORE
{
	render_target::render_target()
	{
		_initialized = init_gl_buffers();
	}

	render_target::render_target(ShaderInfo *shaders, int numShaders) : render_target()
	{
		compile_program(shaders, numShaders);
	}

	render_target::~render_target()
	{
		glDeleteVertexArrays(1, &_VertexArrayID);
		glDeleteBuffers(NumBuffers, _BufferIDs);
	}

	void render_target::compile_program(ShaderInfo *shaders, int numShaders)
	{
		_shaders.clear();

		for (int i = 0; i < numShaders; i++)
		{
			_shaders.push_back(shaders[i]);
		}

		_program = LoadShaders(_shaders.data());
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

	void render_target::draw()
	{
		glUseProgram(_program);

		glBindVertexArray(_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _BufferIDs[ElementBuffer]);

		for (uint32_t i = 0; i < _vertexAttrib.size(); i++)
		{
			const int ID = _vertexAttrib[i].ID;
			const int size = _vertexAttrib[i].size;
			const int type = _vertexAttrib[i].type;
			const int normalized = _vertexAttrib[i].normalized;
			const int stride = _vertexAttrib[i].stride;
			const void *pointer = _vertexAttrib[i].pointer;
			glVertexAttribPointer(ID, size, type, normalized, stride, pointer);
			glEnableVertexAttribArray(ID);
		}
	}

	GLuint render_target::get_program()
	{
		return _program;
	}
}