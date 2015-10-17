#include "billboard.h"

namespace SE_CORE
{
	billboard::billboard(GLfloat width, GLfloat height, int numItems) : render_target()
	{
		_width = width;
		_height = height;
		_numItems = numItems;
	}

	//	1	2	
	//
	//	0	3
	void billboard::gen_vertices()
	{
		const int numVertices = _numItems * 4 * 3;
		GLfloat x_offset = 0.f;
		GLfloat *vBuf = (GLfloat *)malloc(numVertices * sizeof(GLfloat));

		for (int i = 0, n = 0; i < _numItems; i++)
		{
			// 0
			vBuf[n++] = x_offset;
			vBuf[n++] = 0;
			vBuf[n++] = 0;
			// 1
			vBuf[n++] = x_offset;
			vBuf[n++] = _height;
			vBuf[n++] = 0;
			// 2
			vBuf[n++] = x_offset + _width;
			vBuf[n++] = _height;
			vBuf[n++] = 0;
			// 3
			vBuf[n++] = x_offset + _width;
			vBuf[n++] = 0;
			vBuf[n++] = 0;

			x_offset += _width;
		}
	}
}