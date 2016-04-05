#pragma once

#include "common.h"
#include "scene.h"


namespace SE_CORE
{
	bool rayTriangleIntersect(const vec3& org, const vec3& dir, const triangle_t& t);

	class bounding_volume : public render_target
	{
	protected:
		scene_object *_parent;
		bool _updated = false;

		virtual void calcCoords(const dae_reader_t* const reader);
		virtual void init();

	public:
		bounding_volume(scene_object* const parent, const dae_reader_t* const reader, const string& name);
		virtual ~bounding_volume();

		virtual bool intersects(const vec3& org, const vec3& dir) = 0;
		void invalidate();
	};
}