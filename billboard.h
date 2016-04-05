#pragma once

#include "common.h"
#include "rendering.h"

namespace SE_CORE
{
	class billboard : public render_target
	{
	protected:
		GLfloat _width;
		GLfloat _height;
		GLuint _screenWidth;
		GLuint _screenHeight;
		vec2 _pos;

		void init();
		virtual void genVertices();
		virtual void genIndices();
	public:
		billboard(string name);
		~billboard();

		billboard(GLfloat width, GLfloat height, GLuint screenWidth, GLuint screenHeight);
		virtual void render();
		virtual void prepareForRendering(vec2 pos);
	};
}