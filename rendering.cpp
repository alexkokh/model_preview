#include "rendering.h"

namespace SE_CORE
{
	render_target::render_target(string name)
	{
		_visible = true;
		_name = name;
		_vertexBuffer = NULL;
		_indexBuffer = NULL;

		_position = vec3(0, 0, 0);
		_orientation = vec3(0, 0, 1);
		_rotation = mat4::identity();
		_scale = mat4::identity();

		_buffersInitialized = init_gl_buffers();
	}

	render_target::render_target(string name, ShaderInfo *shaders, uint32_t numShaders) : render_target(name)
	{
		compile_program(shaders, numShaders);
	}

	render_target::~render_target()
	{
		if (_vertexBuffer)
			free(_vertexBuffer);

		if (_indexBuffer)
			free(_indexBuffer);

		glDeleteVertexArrays(1, &_VertexArrayID);
		glDeleteBuffers(NumBuffers, _BufferIDs);
	}

	void render_target::compile_program(ShaderInfo *shaders, uint32_t numShaders)
	{
		_shaders.clear();

		for (uint32_t i = 0; i < numShaders; i++)
		{
			_shaders.push_back(shaders[i]);
		}

		_program = LoadShaders(_shaders.data());
	}

	uint32_t render_target::init_gl_buffers()
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

	void render_target::render(camera *cam)
	{
		// bind vertex array and buffers, set vertex attribs
		prepareForRendering();

		GLuint pos_modelview, pos_projection, pos_normal;
		mat4 projection = cam->getProjection();
		mat4 view = cam->getMatrix();

		mat4 modelTranslation = translate(_position);
		mat4 model = modelTranslation * _rotation;

		pos_modelview = glGetUniformLocation(_program, "modelview");
		pos_projection = glGetUniformLocation(_program, "projection");
		pos_normal = glGetUniformLocation(_program, "NormalMatrix");

		mat4 modelview = view * model;

		glUniformMatrix4fv(pos_modelview, 1, false, modelview);
		glUniformMatrix4fv(pos_projection, 1, false, projection);
		glUniformMatrix4fv(pos_normal, 1, false, model);
	}

	bool render_target::getVisible()
	{
		return _visible;
	}

	void render_target::setVisible(bool visible)
	{
		_visible = visible;
	}

	void render_target::prepareForRendering()
	{
		glUseProgram(_program);

		glBindVertexArray(_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _BufferIDs[ElementBuffer]);

		for (uint32_t i = 0; i < _vertexAttrib.size(); i++)
		{
			const uint32_t ID = _vertexAttrib[i].ID;
			const uint32_t size = _vertexAttrib[i].size;
			const uint32_t type = _vertexAttrib[i].type;
			const uint32_t normalized = _vertexAttrib[i].normalized;
			const int32_t stride = _vertexAttrib[i].stride;
			const void *pointer = _vertexAttrib[i].pointer;
			glVertexAttribPointer(ID, size, type, normalized, stride, pointer);
			glEnableVertexAttribArray(ID);
		}
	}

	GLuint render_target::get_program()
	{
		return _program;
	}

	string render_target::getName()
	{
		return _name;
	}

	vec3 render_target::getOrientation()
	{
		return _orientation;
	}

	vec3 render_target::getPosition()
	{
		return _position;
	}

	mat4 render_target::getRotation()
	{
		return _rotation;
	}

	void render_target::setPosition(const vec3& position)
	{
		_position = position;
	}

	void render_target::setRotation(const mat4& rotation)
	{
		_rotation = rotation;
	}
}