#pragma once

#include "common.h"
#include "camera.h"

using namespace std;
using namespace DAE_READER;

namespace SE_CORE
{
	typedef struct vertex_attrib_s
	{
		uint32_t ID;
		uint32_t size;
		uint32_t type;
		uint32_t normalized;
		int32_t stride;
		void *pointer;
	} vertex_attrib_t;

	typedef struct triangle_s {
		vec3 v[3];
	} triangle_t;

	class render_target
	{
	protected:
		string _name;
		bool _visible;
		enum Buffer_IDs { ArrayBuffer, ElementBuffer, NumBuffers };
		enum ElementOffsets { VertexOffset = 0, NormalOffset = 3, TexcoordOffset = 6 };
		vector<ShaderInfo> _shaders;
		vector<vertex_attrib_t> _vertexAttrib;
		GLuint _BufferIDs[NumBuffers];
		GLfloat *_vertexBuffer;
		GLuint *_indexBuffer;
		GLuint _VertexArrayID;
		GLuint _VertexBufferID;
		GLuint _program;
		uint32_t _elementSize;
		uint32_t _buffersInitialized;

		vec3 _position;
		vec3 _orientation;
		mat4 _rotation;
		mat4 _scale;

		uint32_t init_gl_buffers();
		void compile_program(ShaderInfo *shaders, uint32_t numShaders);

	public:
		render_target(string name);
		render_target(string name, ShaderInfo *shaders, uint32_t numShaders);
		virtual ~render_target();
		virtual void prepareForRendering();
		virtual void render(camera *cam);

		GLuint get_program();
		string getName();
		bool getVisible();
		void setVisible(bool visible);

		vec3 getOrientation();
		vec3 getPosition();
		mat4 getRotation();
		virtual void setPosition(const vec3& position);
		virtual void setRotation(const mat4& rotation);
	};
}