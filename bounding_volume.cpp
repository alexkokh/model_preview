#include "bounding_volume.h"
#include "scene.h"

namespace SE_CORE
{
	bounding_volume::bounding_volume(scene_object* const parent, const dae_reader_t* const reader, const string& name) : render_target(name)
	{
		_parent = parent;
	}

	bounding_volume::~bounding_volume()
	{
	}

	void bounding_volume::init()
	{
	}

	void bounding_volume::calcCoords(const dae_reader_t* const reader)
	{
	}

	void bounding_volume::invalidate()
	{
		_updated = true;
	}
}