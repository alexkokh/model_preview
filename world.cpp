#include"world.h"

namespace SE_CORE
{
	world::world()
	{
		_activeCamera = NULL;
		_activeObject = NULL;
	}

	world::~world()
	{
		cleanup();
	}

	void world::addCamera(camera *cam)
	{
		_cameras.push_back(cam);
	}

	void world::addObject(scene_object *object)
	{
		_objects.push_back(object);
	}

	void world::addBillboard(billboard *blbrd)
	{
		_billboards.push_back(blbrd);
	}

	void world::setActiveCamera(camera *cam)
	{
		_activeCamera = cam;
	}

	camera *world::getActiveCamera()
	{
		return _activeCamera;
	}

	void world::setActiveObject(scene_object *object)
	{
		_activeObject = object;
	}

	scene_object *world::getActiveObject()
	{
		return _activeObject;
	}

	scene_object *world::getObject(string name)
	{
		scene_object *obj = NULL;

		for (int32_t i = 0; i < (int32_t)_objects.size(); i++)
		{
			if (name == _objects[i]->getName())
				return _objects[i];
		}

		return NULL;
	}

	void world::processKeyboardInput(unsigned char key)
	{
		if (!_activeObject)
			return;

		switch (key)
		{
		case 111:
			_activeObject->moveForward(1);
			break;

		case 108:
			_activeObject->moveForward(-1);
			break;

		case 97:
			_activeObject->roll(1);
			break;

		case 100:
			_activeObject->roll(-1);
			break;

		case 113:
			_activeObject->yaw(-1);
			break;

		case 101:
			_activeObject->yaw(1);
			break;

		case 119:
			_activeObject->pitch(-1);
			break;

		case 115:
			_activeObject->pitch(1);
			break;
		}
	}

	void world::render()
	{
		// TODO: need frustum and occlusion culling here
		for (int32_t i = 0; i < (int32_t)_objects.size(); i++)
		{
			_objects[i]->render(_activeCamera);
		}

		for (int32_t i = 0; i < (int32_t)_billboards.size(); i++)
		{
			_billboards[i]->render();
		}

		//glFlush();
	}

	void world::cleanup()
	{
		for (int32_t i = 0; i < (int32_t)_objects.size(); i++)
		{
			if (_objects[i])
				delete _objects[i];
		}

		_objects.clear();

		for (int32_t i = 0; i < (int32_t)_billboards.size(); i++)
		{
			if (_billboards[i])
				delete _billboards[i];
		}

		_billboards.clear();

		for (int32_t i = 0; i < (int32_t)_cameras.size(); i++)
		{
			if (_cameras[i])
				delete _cameras[i];
		}

		_cameras.clear();
	}
}