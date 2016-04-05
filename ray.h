#pragma once

#include "common.h"
#include "scene.h"

namespace SE_CORE
{
	class ray : public scene_object
	{
	protected:
		GLfloat _lineWidth;
		vec3 _origin;
		vec3 _direction;
		vec4 _color;

		void init();
		virtual void genVertices();

	public:
		ray(string name, vec3 origin, vec3 direction, GLfloat lineWidth, vec4 color);
		virtual ~ray();

		virtual void render(camera *cam);
		void setCoords(vec3 origin, vec3 direction);
		void setLineWidth(GLfloat lineWidth);
		vec3 getOrigin();
		vec3 getDirection();
	};
}
