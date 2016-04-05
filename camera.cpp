#include "camera.h"

namespace SE_CORE
{
	camera::camera(string name, float fov, float aspectRatio, float zNear, float zFar)
	{
		_name = name;
		_fov = fov;
		_projection = perspective(fov, aspectRatio, zNear, zFar);
		_matrix = mat4::identity();
	}
	
	void camera::lookAtRH(vec3 eye, vec3 target, vec3 up)
	{
		_pos = eye;
		_lookAt = target;
		_up = up;

		vec3 zaxis = normalize(eye - target);    // The "forward" vector.
		vec3 xaxis = normalize(cross(up, zaxis));// The "right" vector.
		vec3 yaxis = cross(zaxis, xaxis);     // The "up" vector.

											  // Create a 4x4 orientation matrix from the right, up, and forward vectors
											  // This is transposed which is equivalent to performing an inverse 
											  // if the matrix is orthonormalized (in this case, it is).
		mat4 orientation = {
			vec4(xaxis[0], yaxis[0], zaxis[0], 0),
			vec4(xaxis[1], yaxis[1], zaxis[1], 0),
			vec4(xaxis[2], yaxis[2], zaxis[2], 0),
			vec4(0, 0, 0, 1)
		};

		// Create a 4x4 translation matrix.
		// The eye position is negated which is equivalent
		// to the inverse of the translation matrix. 
		// T(v)^-1 == T(-v)
		mat4 translation = {
			vec4(1, 0, 0, 0),
			vec4(0, 1, 0, 0),
			vec4(0, 0, 1, 0),
			vec4(-eye[0], -eye[1], -eye[2], 1)
		};

		// Combine the orientation and translation to compute 
		// the final view matrix
		_matrix = orientation * translation;
	}

	void camera::FPSViewRH(vec3 pos, vec3 eulerAngles)
	{
		mat4 cam = vmath::rotate(eulerAngles[0], vec3(0, 1, 0)); // yaw
		cam *= translate(vec3(-pos[0], 0, -pos[2]));

		_matrix = cam;
	}

	mat4 camera::getMatrix()
	{
		return _matrix;
	}

	mat4 camera::getProjection()
	{
		return _projection;
	}

	vec3 camera::getLookAt()
	{
		return _lookAt;
	}

	vec3 camera::getPos()
	{
		return _pos;
	}

	quaternion camera::getOffset(vec3 axis, float angle)
	{
		axis = normalize(axis);
		axis *= sin(angle / 2.0f);
		float scalar = cos(angle / 2.0f);
		quaternion offset = quaternion(axis[0], axis[1], axis[2], scalar);

		return offset;
	}

	void camera::rotate(float xzDelta, float yDelta)
	{
		quaternion offset;
		vec3 dif = _pos - _lookAt;
		float delta = abs(.04f * dif[1]); // 4% allowance

		offset = getOffset(vec3(0, 1, 0), xzDelta);

		// prevent the XZ plane from flipping due to the camera's vertical orientation
		if (abs(dif[0]) < delta && abs(dif[2]) < delta &&
			((dif[1] > 0 && yDelta > 0) ||
				(dif[1] < 0 && yDelta < 0)))
		{
			yDelta = 0;
		}
		else
		{
			vec3 v = normalize(vec3(_pos[0], 0, _pos[2]));
			vec3 perpendicularAxis = cross(vec3(0, 1, 0), v); // axis, perpendicular to the camera vector in XZ plane

			quaternion yOffset = getOffset(perpendicularAxis, yDelta);
			offset = offset * yOffset;
		}

		mat4 pos;
		pos = offset.asMatrix() * vec4(_pos, 1);
		_pos[0] = pos[0][0];
		_pos[1] = pos[0][1];
		_pos[2] = pos[0][2];

		lookAtRH(_pos, _lookAt, _up);
	}

	void camera::moveForward(float delta)
	{
		vec3 direction = normalize(_pos - _lookAt);
		vec3 offset = direction *= delta;

		_pos += offset;
		lookAtRH(_pos, _lookAt, _up);
	}
}