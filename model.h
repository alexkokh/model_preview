#pragma once

#include "vmath.h"
#include "rendering.h"
#include "DAEReader.h"

namespace SE_CORE
{
	class model
	{
	private:
		vmath::vec3 m_Position;
		vmath::vec3 m_Rotation;
		vmath::vec3 m_Scale;
		render_target m_RenderTarget;
	public:
		model(char *path);
		void draw();
	};
}