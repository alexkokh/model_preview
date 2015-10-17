#pragma once

#include "common.h"

using namespace std;
using namespace DAE_READER;

namespace SE_CORE
{
	typedef struct vertex_attrib_s
	{
		int ID;
		int size;
		int type;
		int normalized;
		int stride;
		void *pointer;
	} vertex_attrib_t;

	class render_target
	{
	protected:
		enum Buffer_IDs { ArrayBuffer, ElementBuffer, NumBuffers };
		enum ElementOffsets { VertexOffset = 0, NormalOffset = 3, TexcoordOffset = 6 };
		vector<vertex_attrib_t> _vertexAttrib;
		GLuint _BufferIDs[NumBuffers];
		GLuint _VertexArrayID;
		GLuint _VertexBufferID;
		int _ElementSize;
		int _initialized;

		int init_gl_buffers();

	public:
		render_target();
		virtual ~render_target();
		void draw();
	};
}