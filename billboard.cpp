#include "billboard.h"

namespace SE_CORE
{
	billboard::billboard(GLfloat width, GLfloat height, int numItems, GLuint screenWidth, GLuint screenHeight) : render_target()
	{
		_width = width;
		_height = height;
		_numItems = numItems;
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;
		_elementSize = 5; // x, y, z, s, t

		if (_initialized)
		{
			ShaderInfo shaders[] = {
				{ GL_VERTEX_SHADER, "font_vs.glsl" },
				{ GL_FRAGMENT_SHADER, "font_fs.glsl" },
				{ GL_NONE, NULL }
			};

			compile_program(shaders, 3);
			gen_vertices();
		}
	}

	//	1	2	
	//
	//	0	3
	void billboard::gen_vertices()
	{
		const int numVertices = _numItems * 4;
		const int numIndices = _numItems * 6;
		const int vBufSize = numVertices * _elementSize * sizeof(GLfloat);
		const int iBufSize = numIndices * sizeof(GLint);
		GLfloat *vBuf = (GLfloat *)malloc(vBufSize);
		GLuint *iBuf = (GLuint *)malloc(iBufSize);
		GLfloat x_offset = 0.f;

		for (int i = 0, n = 0, k = 0; i < _numItems; i++)
		{
			// 0
			vBuf[n++] = x_offset;
			vBuf[n++] = 0;
			vBuf[n++] = 0;
			vBuf[n++] = 0; // s
			vBuf[n++] = 0; // t

			// 1
			vBuf[n++] = x_offset;
			vBuf[n++] = _height;
			vBuf[n++] = 0;
			vBuf[n++] = 0; // s
			vBuf[n++] = 1; // t

			// 2
			vBuf[n++] = x_offset + _width;
			vBuf[n++] = _height;
			vBuf[n++] = 0;
			vBuf[n++] = 1; // s
			vBuf[n++] = 1; // t

			// 3
			vBuf[n++] = x_offset + _width;
			vBuf[n++] = 0;
			vBuf[n++] = 0;
			vBuf[n++] = 1; // s
			vBuf[n++] = 0; // t

			int ind_offset = i * 4;
			iBuf[k++] = ind_offset + 0;
			iBuf[k++] = ind_offset + 1;
			iBuf[k++] = ind_offset + 3;
			iBuf[k++] = ind_offset + 1;
			iBuf[k++] = ind_offset + 2;
			iBuf[k++] = ind_offset + 3;
			x_offset += _width;
		}

		glBindVertexArray(_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBufferData(GL_ARRAY_BUFFER, vBufSize, vBuf, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _BufferIDs[ElementBuffer]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iBufSize, iBuf, GL_STATIC_DRAW);

		free(vBuf);
		free(iBuf);
	}

	void billboard::draw(char *str, GLfloat x, GLfloat y)
	{
		render_target::draw();

		int stride = _elementSize * sizeof(float);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)12);
		glEnableVertexAttribArray(1);

		mat4 modelview = mat4::identity();
		mat4 ortho_proj = ortho(0, (float)_screenWidth, (float)_screenHeight, 0, -1, 15);

		GLuint pos_modelview = glGetUniformLocation(_program, "modelview");
		GLuint pos_projection = glGetUniformLocation(_program, "projection");
		glUniformMatrix4fv(pos_modelview, 1, false, modelview);
		glUniformMatrix4fv(pos_projection, 1, false, ortho_proj);

		GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

		if(depthTestEnabled)
			glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, _numItems * 6, GL_UNSIGNED_INT, 0);
		
		if(depthTestEnabled)
			glEnable(GL_DEPTH_TEST);
	}
}