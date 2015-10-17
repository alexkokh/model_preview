#pragma once

#include "common.h"
#include "rendering.h"

namespace SE_CORE
{
	class billboard : protected render_target
	{
	private:
		GLfloat _width;
		GLfloat _height;
		int _numItems;
		vec3 pos;

		void init();
		void gen_vertices();
	public:
		billboard(GLfloat width, GLfloat height, int numItems);
		void draw(char *str, GLfloat x, GLfloat y);
	};
}