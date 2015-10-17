#include "common.h"

using namespace std;
using namespace DAE_READER;

namespace SE_CORE
{
	class render_target
	{
	private:
		enum Buffer_IDs { ArrayBuffer, ElementBuffer, NumBuffers };
		enum ElementOffsets { VertexOffset = 0, NormalOffset = 3, TexcoordOffset = 6 };
		GLuint _BufferIDs[NumBuffers];
		GLuint _VertexArrayID;
		GLuint _VertexBufferID;
		int _ElementSize;
		vector<vector<vector<draw_range_t>>> _MeshDrawRange;

		int init_gl_buffers();

	public:
		render_target();
		~render_target();
		int load(char *path);
		void draw();
	};
}