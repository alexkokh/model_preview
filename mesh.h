#pragma once

#include "common.h"
#include "scene.h"
#include "camera.h"
#include "bounding_box.h"

using namespace std;
using namespace DAE_READER;

namespace SE_CORE
{
	class mesh : public scene_object
	{
	private:
		vector<vector<vector<draw_range_t>>> _MeshDrawRange;
	public:
		mesh();
		mesh(string name, ShaderInfo *shaders, uint32_t numShaders);
		virtual ~mesh();

		uint32_t load(char *path);
		virtual void render(camera *cam);
	};
}