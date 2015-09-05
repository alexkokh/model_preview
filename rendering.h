#define FREEGLUT_STATIC

#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\glext.h>
#include <gl\freeglut.h>
#include <vector>
#include "DAEReader.h"

using namespace std;
using namespace DAE_READER;

namespace SE_CORE
{
	class render_target
	{
	private:
		int x;
		int y;
		int z;
		enum Buffer_IDs { ArrayBuffer, ElementBuffer, NumBuffers };
		enum ElementOffsets { VertexOffset = 0, NormalOffset = 3, TexcoordOffset = 6 };
		GLuint m_BufferIDs[NumBuffers];
		GLuint m_VertexArrayID;
		GLuint m_VertexBufferID;
		int m_ElementSize;
		vector<vector<vector<draw_range_t>>> m_MeshDrawRange;

		int init_gl_buffers();

	public:
		render_target();
		~render_target();
		int load(char *path);
		void draw();
	};
}