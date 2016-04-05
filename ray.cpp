#include "ray.h"

namespace SE_CORE
{
	ray::ray(string name, vec3 origin, vec3 direction, GLfloat lineWidth, vec4 color) : scene_object(name)
	{
		_lineWidth = lineWidth;
		_elementSize = 3;
		_origin = origin;
		_direction = direction;
		_color = color;
		const uint32_t numVertices = 2;
		const uint32_t vBufSize = numVertices * _elementSize * sizeof(GLfloat);
		_vertexBuffer = (GLfloat *)malloc(vBufSize);

		const int32_t stride = _elementSize * sizeof(float);
		vertex_attrib_t coords = { 0, 3, GL_FLOAT, GL_FALSE, stride, 0 };
		_vertexAttrib.push_back(coords);

		init();
	}

	void ray::init()
	{
		if (_buffersInitialized)
		{
			ShaderInfo shaders[] = {
				{ GL_VERTEX_SHADER, "ray_vs.glsl" },
				{ GL_FRAGMENT_SHADER, "ray_fs.glsl" },
				{ GL_NONE, NULL }
			};

			compile_program(shaders, 3);
			genVertices();
		}
	}

	ray::~ray()
	{
	}

	void ray::genVertices()
	{
		const uint32_t numVertices = 2;
		const uint32_t vBufSize = numVertices * _elementSize * sizeof(GLfloat);
		uint32_t n = 0;
		_vertexBuffer[n++] = _origin[0];
		_vertexBuffer[n++] = _origin[1];
		_vertexBuffer[n++] = _origin[2];

		_vertexBuffer[n++] = _direction[0];
		_vertexBuffer[n++] = _direction[1];
		_vertexBuffer[n++] = _direction[2];

		glBindVertexArray(_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBufferData(GL_ARRAY_BUFFER, vBufSize, _vertexBuffer, GL_DYNAMIC_DRAW);
	}

	void ray::render(camera *cam)
	{
		if (!_visible)
			return;

		GLuint colorPos;
		scene_object::render(cam);
		colorPos = glGetUniformLocation(_program, "inColor");
		glUniform4fv(colorPos, 1, _color);

		glLineWidth(_lineWidth);
		glDrawArrays(GL_POINTS, 0, 1);
		glDrawArrays(GL_LINES, 0, 2);
	}

	void ray::setCoords(vec3 origin, vec3 direction)
	{
		_origin = origin;
		_direction = direction;
		genVertices();
	}

	void ray::setLineWidth(GLfloat lineWidth)
	{
		_lineWidth = lineWidth;
	}

	vec3 ray::getOrigin()
	{
		return _origin;
	}

	vec3 ray::getDirection()
	{
		return _direction;
	}
}