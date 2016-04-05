#include "scene.h"
#include "bounding_volume.h"

namespace SE_CORE
{
	scene_object::scene_object(const string& name) : render_target(name)
	{
		_boundingVolume = nullptr;
		initialize();
	}

	scene_object::scene_object(const string& name, ShaderInfo *shaders, uint32_t numShaders) : render_target(name, shaders, numShaders)
	{
		initialize();
	}

	scene_object::~scene_object()
	{
		if (_boundingVolume)
			delete _boundingVolume;
	}

	void scene_object::initialize()
	{
		_q = quaternion(0, 0, 0, 1);
	}

	bool scene_object::intersects(const vec3& org, const vec3& dir)
	{
		return _boundingVolume->intersects(org, dir);
	}

	void scene_object::render(camera *cam)
	{
		render_target::render(cam);
		GLuint posAlpha;
		posAlpha = glGetUniformLocation(_program, "alpha");

		glUniform1f(posAlpha, _alpha);
	}

	void scene_object::calcOrientation()
	{
		// assumes that the object is initially aligned with the z axis
		vec4 initialOrientation(0, 0, 1, 0);
		mat4 m = _rotation * initialOrientation;

		_orientation = vec3(m[0][0], m[0][1], m[0][2]);
	}

	quaternion scene_object::calcQuaternionOffset(vec3 axis, float angle)
	{
		angle /= ONE_RADIAN;
		axis = normalize(axis);
		axis *= sin(angle / 2.0f);
		float scalar = cos(angle / 2.0f);
		quaternion offset = quaternion(axis[0], axis[1], axis[2], scalar);

		return offset;
	}

	void scene_object::setPosition(const vec3& position)
	{
		render_target::setPosition(position);
		_boundingVolume->invalidate();
	}

	void scene_object::setRotation(const mat4& rotation)
	{
		render_target::setRotation(rotation);
		_boundingVolume->invalidate();
	}

	void scene_object::setBoundingVolumeVisible(bool visible)
	{
		_boundingVolume->setVisible(visible);
	}

	void scene_object::setAlpha(GLfloat alpha)
	{
		_alpha = alpha;
	}

	void scene_object::moveForward(float delta)
	{
		_position += delta*_orientation;
		_boundingVolume->invalidate();
	}

	void scene_object::rotate(vec3 axis, float angle)
	{
		_rotation = vmath::rotate(angle, axis);
		_q = quaternion(vec4(axis, 0));
		_boundingVolume->invalidate();

		_ASSERT(_rotation == _q.asMatrix());
	}

	void scene_object::rotateQ(vec3 axis, float angle, float absolute)
	{
		quaternion offset = calcQuaternionOffset(axis, angle);

		if (!absolute)
			_q = offset * _q;

		_q = normalize(_q);

		_rotation = _q.asMatrix();
		calcOrientation();
		_boundingVolume->invalidate();
	}

	void scene_object::pitch(float angle, bool absolute)
	{
		vec3 axis = vec3(1, 0, 0);
		rotateQ(axis, angle, absolute);
	}

	void scene_object::pitch(float angle)
	{
		pitch(angle, false);
	}

	void scene_object::pitchAbs(float angle)
	{
		pitch(angle, true);
	}

	void scene_object::roll(float angle, bool absolute)
	{
		vec3 axis = vec3(0, 0, 1);
		rotateQ(axis, angle, absolute);
	}

	void scene_object::roll(float angle)
	{
		roll(angle, false);
	}

	void scene_object::rollAbs(float angle)
	{
		roll(angle, true);
	}

	void scene_object::yaw(float angle, bool absolute)
	{
		vec3 axis = vec3(0, 1, 0);
		rotateQ(axis, angle, absolute);
	}

	void scene_object::yaw(float angle)
	{
		yaw(angle, false);
	}

	void scene_object::yawAbs(float angle)
	{
		yaw(angle, true);
	}
}