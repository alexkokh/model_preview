#pragma once

#include "rendering.h"
#include "camera.h"

namespace SE_CORE
{
#define ONE_RADIAN	(float)(180.f/M_PI)
	class bounding_volume;

	enum draw_mode_e
	{
		draw_mode_wireframe = 0,
		draw_mode_polygon
	};

	class intersect_test
	{
		virtual bool intersects(const vec3& org, const vec3& dir) = 0;
	};

	class scene_object : public render_target, public intersect_test
	{
	protected:
		quaternion _q;

		bounding_volume *_boundingVolume;
		draw_mode_e _bv_draw_mode = draw_mode_wireframe;
		GLfloat _alpha = 1;

		virtual void initialize();
		void calcOrientation();

		// returns offset quaternion corresponding to the rotation around the specified axis by the given amount of degrees
		quaternion calcQuaternionOffset(vec3 axis, float angle);

		void rotateQ(vec3 axis, float angle, float absolute);
	public:
		scene_object(const string& name);
		scene_object(const string& name, ShaderInfo *shaders, uint32_t numShaders);
		virtual ~scene_object();

		virtual void render(camera *cam);
		virtual bool intersects(const vec3& org, const vec3& dir) override;

		void setBoundingVolumeVisible(bool visible);
		void setBoundingVolumeDrawMode(draw_mode_e mode);

		virtual void setPosition(const vec3& position) override;
		virtual void setRotation(const mat4& rotation) override;

		void setAlpha(GLfloat alpha);

		// rotates the object arond specified vector in degrees
		virtual void rotate(vec3 axis, float angle);

		// changes pitch in degrees
		virtual void pitch(float angle, bool absolute);
		// incrementally changes pitch  in degrees
		virtual void pitch(float angle);
		// sets absolute pitch in degrees
		virtual void pitchAbs(float angle);

		// sets roll in degrees
		virtual void roll(float angle, bool absolute);
		// incrementally sets roll in degrees
		virtual void roll(float angle);
		// set absolute bank angle in degrees
		virtual void rollAbs(float angle);

		// sets yaw in degrees
		virtual void yaw(float angle, bool absolute);
		// incrementally sets yaw in degrees
		virtual void yaw(float angle);
		// sets absolute yaw
		virtual void yawAbs(float angle);

		virtual void moveForward(float delta);
	};
}