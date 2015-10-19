#pragma once

#include "common.h"
#include "rendering.h"

namespace SE_CORE
{
	class billboard : public render_target
	{
	private:
		GLfloat _width;
		GLfloat _height;
		GLuint _screenWidth;
		GLuint _screenHeight;
		int _numItems;
		int _elementSize;
		vec3 _pos;

		void init();
		void gen_vertices();
	public:
		billboard(GLfloat width, GLfloat height, int numItems, GLuint screenWidth, GLuint screenHeight);
		void draw(char *str, GLfloat x, GLfloat y);
	};
}