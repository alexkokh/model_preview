#include "billboard.h"

namespace SE_CORE
{
	// does nothing, should only be called from the derived classes
	billboard::billboard(string name) : render_target(name)
	{
	}

	billboard::billboard(GLfloat width, GLfloat height, GLuint screenWidth, GLuint screenHeight) : render_target("")
	{
		_width = width;
		_height = height;
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;
		_elementSize = 5; // x, y, z, s, t

		init();
	}

	billboard::~billboard()
	{
	}

	void billboard::init()
	{
		if (_buffersInitialized)
		{
			ShaderInfo shaders[] = {
				{ GL_VERTEX_SHADER, "font_vs.glsl" },
				{ GL_FRAGMENT_SHADER, "font_fs.glsl" },
				{ GL_NONE, NULL }
			};

			compile_program(shaders, 3);
			genVertices();
			genIndices();
		}
	}

	//	1	2	
	//
	//	0	3
	void billboard::genVertices()
	{
		const uint32_t numVertices = 4*_elementSize;
		const uint32_t vBufSize = numVertices * _elementSize * sizeof(GLfloat);
		GLfloat x_offset = 0.f;
		_vertexBuffer = (GLfloat *)malloc(vBufSize);

		// 0
		uint32_t i = 0;
		uint32_t n = 0;
		uint32_t k = 0;
		_vertexBuffer[n++] = x_offset;
		_vertexBuffer[n++] = 0;
		_vertexBuffer[n++] = 0;
		_vertexBuffer[n++] = 0; // s
		_vertexBuffer[n++] = 0; // t

		// 1
		_vertexBuffer[n++] = x_offset;
		_vertexBuffer[n++] = _height;
		_vertexBuffer[n++] = 0;
		_vertexBuffer[n++] = 0; // s
		_vertexBuffer[n++] = 1; // t

		// 2
		_vertexBuffer[n++] = x_offset + _width;
		_vertexBuffer[n++] = _height;
		_vertexBuffer[n++] = 0;
		_vertexBuffer[n++] = 1; // s
		_vertexBuffer[n++] = 1; // t

		// 3
		_vertexBuffer[n++] = x_offset + _width;
		_vertexBuffer[n++] = 0;
		_vertexBuffer[n++] = 0;
		_vertexBuffer[n++] = 1; // s
		_vertexBuffer[n++] = 0; // t

		x_offset += _width;

		glBindVertexArray(_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBufferData(GL_ARRAY_BUFFER, vBufSize, _vertexBuffer, GL_STATIC_DRAW);
	}

	void billboard::genIndices()
	{
		const uint32_t numIndices = 6;
		const uint32_t iBufSize = numIndices * sizeof(GLint);
		GLuint iBuf[numIndices];

		uint32_t i = 0;
		uint32_t k = 0;
		uint32_t ind_offset = i * 4;

		iBuf[k++] = ind_offset + 0;
		iBuf[k++] = ind_offset + 1;
		iBuf[k++] = ind_offset + 3;
		iBuf[k++] = ind_offset + 1;
		iBuf[k++] = ind_offset + 2;
		iBuf[k++] = ind_offset + 3;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _BufferIDs[ElementBuffer]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iBufSize, iBuf, GL_STATIC_DRAW);
	}

	void billboard::prepareForRendering(vec2 pos)
	{
		render_target::prepareForRendering();
		uint32_t stride = _elementSize * sizeof(float);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)12);
		glEnableVertexAttribArray(1);

		mat4 modelview = translate(vec3(pos[0], pos[1], 0));
		mat4 ortho_proj = ortho(0, (float)_screenWidth, (float)_screenHeight, 0, -1, 15);

		GLuint pos_modelview = glGetUniformLocation(_program, "modelview");
		GLuint pos_projection = glGetUniformLocation(_program, "projection");
		glUniformMatrix4fv(pos_modelview, 1, false, modelview);
		glUniformMatrix4fv(pos_projection, 1, false, ortho_proj);
	}

	void billboard::render()
	{
		prepareForRendering(_pos);

		GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
		if(depthTestEnabled)
			glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		if(depthTestEnabled)
			glEnable(GL_DEPTH_TEST);
	}
}