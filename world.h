#pragma once

#include "common.h"
#include "camera.h"
#include "rendering.h"
#include "scene.h"
#include "mesh.h"
#include "billboard.h"

namespace SE_CORE
{
	class world
	{
	private:
		vector<camera *> _cameras;
		vector<scene_object *> _objects;
		vector<billboard *> _billboards;
		camera *_activeCamera;
		scene_object *_activeObject;

	public:
		world();
		~world();

		void addCamera(camera *cam);
		void addObject(scene_object *object);
		void addBillboard(billboard *blbrd);
		scene_object *getObject(string name);
		scene_object *getActiveObject();
		void setActiveObject(scene_object *object);

		camera *getActiveCamera();
		camera *getCamera(string name);
		vector<camera *> *getCameras();
		void setActiveCamera(camera *cam);

		void processKeyboardInput(unsigned char key);

		void render();
		void cleanup();
	};
}