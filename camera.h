#pragma once

#include "common.h"

namespace SE_CORE
{
	class camera
	{
	private:
		string _name;
		float _fov;
		mat4 _matrix;
		mat4 _projection;
		vec3 _pos;
		vec3 _lookAt;
		vec3 _up;

		quaternion getOffset(vec3 axis, float angle);

	public:
		camera(string name, float fov, float aspectRatio, float zNear, float zFar);
		void lookAtRH(vec3 eye, vec3 target, vec3 up);
		void FPSViewRH(vec3 pos, vec3 eulerAngles);
		void rotate(float xzDelta, float yDelta);
		void moveForward(float delta);

		mat4 getMatrix();
		mat4 getProjection();
		vec3 getLookAt();
		vec3 getPos();
	};
}