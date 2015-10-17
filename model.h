#pragma once

#include "vmath.h"
#include "rendering.h"
#include "DAEReader.h"

namespace SE_CORE
{
	class model
	{
	private:
		vmath::vec3 _Position;
		vmath::vec3 _Rotation;
		vmath::vec3 _Scale;
		render_target _RenderTarget;
	public:
		model(char *path);
		void draw();
	};
}