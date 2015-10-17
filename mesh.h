#pragma once

#include "common.h"
#include "rendering.h"

using namespace std;
using namespace DAE_READER;

namespace SE_CORE
{
	class mesh : protected render_target
	{
	private:
		vector<vector<vector<draw_range_t>>> _MeshDrawRange;
	public:
		mesh();
		virtual ~mesh();

		int load(char *path);
		void draw();
	};
}