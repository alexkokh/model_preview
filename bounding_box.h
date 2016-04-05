#pragma once

#include "bounding_volume.h"

namespace SE_CORE
{
	class bounding_box : public bounding_volume
	{
	private:
		triangle_t _triangles[12];
		vec3 _vertices[8];

		virtual void init();
		virtual void calcCoords(const dae_reader_t* const reader);
		void findMinMax(const dae_reader_t* const reader, float *min, float *max);
		void mapVertices();
		void mapTriangles();

	public:
		bounding_box(scene_object* const parent, const dae_reader_t* const reader, const string& name);
		virtual ~bounding_box();

		virtual void render(camera *cam);
		virtual bool intersects(const vec3& org, const vec3& dir) override;
	};
}