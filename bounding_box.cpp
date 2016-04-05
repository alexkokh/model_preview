#pragma once

#include "bounding_box.h"
#include "se_math.h"

namespace SE_CORE
{
	bounding_box::bounding_box(scene_object* const parent, const dae_reader_t* const reader, const string& name) : bounding_volume(parent, reader, name + "_boundingbox")
	{
		_visible = false;
		_elementSize = 3;
		init();
		calcCoords(reader);

		const int32_t stride = _elementSize * sizeof(float);
		vertex_attrib_t coords = { 0, 3, GL_FLOAT, GL_FALSE, stride, 0 };
		_vertexAttrib.push_back(coords);
	}

	bounding_box::~bounding_box()
	{
	}

	void bounding_box::init()
	{
		if (_buffersInitialized)
		{
			ShaderInfo shaders[] = {
				{ GL_VERTEX_SHADER, "boundingvolume_vs.glsl" },
				{ GL_FRAGMENT_SHADER, "boundingvolume_fs.glsl" },
				{ GL_NONE, NULL }
			};

			compile_program(shaders, 3);
		}
	}

	void bounding_box::findMinMax(const dae_reader_t* const reader, float *min, float *max)
	{
		const float fmin = numeric_limits<float>::max();
		const float fmax = numeric_limits<float>::min();		
		
		for (int32_t i = 0; i < 3; i++)
		{
			min[i] = fmin;
			max[i] = fmax;
		}

		for (uint32_t i = 0; i < reader->geometry.size(); i++)
		{
			const vector<float>& v = reader->geometry[i].vertices;
			_ASSERT(!(v.size() % 3));

			for (uint32_t j = 0; j < v.size(); j+=3)
			{
				for (uint32_t k = 0; k < 3; k++)
				{
					float val = v[j + k];
					if (val > max[k])
						max[k] = val;

					if (val < min[k])
						min[k] = val;
				}
			}
		}
	}

	void bounding_box::mapVertices()
	{
		_vertices[0] = vec3(_vertexBuffer[0],  _vertexBuffer[1],  _vertexBuffer[2]);  // 0
		_vertices[1] = vec3(_vertexBuffer[3],  _vertexBuffer[4],  _vertexBuffer[5]);  // 1
		_vertices[2] = vec3(_vertexBuffer[6],  _vertexBuffer[7],  _vertexBuffer[8]);  // 2
		_vertices[3] = vec3(_vertexBuffer[9],  _vertexBuffer[10], _vertexBuffer[11]); // 3
		_vertices[4] = vec3(_vertexBuffer[12], _vertexBuffer[13], _vertexBuffer[14]); // 4
		_vertices[5] = vec3(_vertexBuffer[15], _vertexBuffer[16], _vertexBuffer[17]); // 5
		_vertices[6] = vec3(_vertexBuffer[18], _vertexBuffer[19], _vertexBuffer[20]); // 6
		_vertices[7] = vec3(_vertexBuffer[21], _vertexBuffer[22], _vertexBuffer[23]); // 7
	}

	void bounding_box::mapTriangles()
	{
		// back face
		_triangles[0].v[0] = _vertices[0]; // 0
		_triangles[0].v[1] = _vertices[1]; // 1
		_triangles[0].v[2] = _vertices[2]; // 2
							 
		_triangles[1].v[0] = _vertices[0]; // 0
		_triangles[1].v[1] = _vertices[2]; // 2
		_triangles[1].v[2] = _vertices[3]; // 3

		// front face
		_triangles[2].v[0] = _vertices[4]; // 4
		_triangles[2].v[1] = _vertices[6]; // 6
		_triangles[2].v[2] = _vertices[5]; // 5
							 
		_triangles[3].v[0] = _vertices[4]; // 4
		_triangles[3].v[1] = _vertices[7]; // 7
		_triangles[3].v[2] = _vertices[6]; // 6

		// top face
		_triangles[4].v[0] = _vertices[7]; // 7
		_triangles[4].v[1] = _vertices[3]; // 3
		_triangles[4].v[2] = _vertices[2]; // 2
							 
		_triangles[5].v[0] = _vertices[7]; // 7
		_triangles[5].v[1] = _vertices[2]; // 2
		_triangles[5].v[2] = _vertices[6]; // 6

		// bottom face
		_triangles[6].v[0] = _vertices[4]; // 4
		_triangles[6].v[1] = _vertices[1]; // 1
		_triangles[6].v[2] = _vertices[0]; // 0
							 
		_triangles[7].v[0] = _vertices[4]; // 4
		_triangles[7].v[1] = _vertices[5]; // 5
		_triangles[7].v[2] = _vertices[1]; // 1

		// left face
		_triangles[8].v[0] = _vertices[0]; // 0
		_triangles[8].v[1] = _vertices[3]; // 3
		_triangles[8].v[2] = _vertices[7]; // 7
							 
		_triangles[9].v[0] = _vertices[0]; // 0
		_triangles[9].v[1] = _vertices[7]; // 7
		_triangles[9].v[2] = _vertices[4]; // 4

		// right face
		_triangles[10].v[0] = _vertices[5]; // 5
		_triangles[10].v[1] = _vertices[6]; // 6
		_triangles[10].v[2] = _vertices[2]; // 2
							  
		_triangles[11].v[0] = _vertices[5]; // 5
		_triangles[11].v[1] = _vertices[2]; // 2
		_triangles[11].v[2] = _vertices[1]; // 1
	}

	void bounding_box::calcCoords(const dae_reader_t* const reader)
	{
		float min[3];
		float max[3];
		const int32_t numVertices = 8*3;
		const int32_t numIndices = 8;
		const int32_t vBufSize = numVertices * sizeof(float);

		findMinMax(reader, min, max);

		_vertexBuffer = (float *)malloc(vBufSize);

		//		3-----------2
		//	   /|          /|
		//    / |         / |
		//   /  0--------/--1
		//  7--/--------6  /
		//  | /         | /
		//  |/          |/
		//  4-----------5


		_vertexBuffer[0]  = min[0]; _vertexBuffer[1]  = min[2]; _vertexBuffer[2]  = min[1]; // 0
		_vertexBuffer[3]  = max[0]; _vertexBuffer[4]  = min[2]; _vertexBuffer[5]  = min[1]; // 1
		_vertexBuffer[6]  = max[0]; _vertexBuffer[7]  = max[2]; _vertexBuffer[8]  = min[1]; // 2
		_vertexBuffer[9]  = min[0]; _vertexBuffer[10] = max[2]; _vertexBuffer[11] = min[1]; // 3
		_vertexBuffer[12] = min[0]; _vertexBuffer[13] = min[2]; _vertexBuffer[14] = max[1]; // 4
		_vertexBuffer[15] = max[0]; _vertexBuffer[16] = min[2]; _vertexBuffer[17] = max[1]; // 5
		_vertexBuffer[18] = max[0]; _vertexBuffer[19] = max[2]; _vertexBuffer[20] = max[1]; // 6
		_vertexBuffer[21] = min[0]; _vertexBuffer[22] = max[2]; _vertexBuffer[23] = max[1]; // 7

		mapVertices();
		mapTriangles();

		glBindVertexArray(_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBufferData(GL_ARRAY_BUFFER, vBufSize, _vertexBuffer, GL_STATIC_DRAW);

		/*const int32_t triSize = 12 * 3 * 3 * sizeof(GLfloat);
		glBufferData(GL_ARRAY_BUFFER, triSize, NULL, GL_STATIC_DRAW);

		for (uint32_t i = 0, offset = 0; i < 12; i++)
		{
			for (uint32_t j = 0; j < 3; j++)
			{
				const int32_t size = 3 * sizeof(GLfloat);
				glBufferSubData(GL_ARRAY_BUFFER, offset, size, (const float *)_triangles[i].v[j]);
				offset += size;
			}
		}*/

		const int32_t iBufSize = 18 * sizeof(GLuint);
		_indexBuffer = (GLuint *)malloc(iBufSize);

		_indexBuffer[0] = 0; _indexBuffer[1] = 1; _indexBuffer[2] = 2; _indexBuffer[3] = 3; _indexBuffer[4] = 0;
		_indexBuffer[5] = 4; _indexBuffer[6] = 5; _indexBuffer[7] = 6; _indexBuffer[8] = 7; _indexBuffer[9] = 4;
		
		for (int32_t i = 0; i < 4; i++)
		{
			_indexBuffer[i*2 + 10] = i;
			_indexBuffer[i*2 + 11] = i + 4;
		}

		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ElementBuffer]);
		glBufferData(GL_ARRAY_BUFFER, iBufSize, _indexBuffer, GL_STATIC_DRAW);
	}

	void bounding_box::render(camera *cam)
	{
		if (!_visible) return;
		render_target::render(cam);
		glPointSize(5);
/*
		for (uint32_t i = 0; i < 12; i++)
		{
			glDrawArrays(GL_LINE_LOOP, i*3, 3);
		}
*/
		
		glDrawElements(GL_LINE_LOOP, 5, GL_UNSIGNED_INT, 0);
		glDrawElements(GL_LINE_LOOP, 5, GL_UNSIGNED_INT, (void *)(5*sizeof(GLuint)));

		for (int32_t i = 0; i < 4; i ++)
		{
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void *)((10 + i*2) * sizeof(GLuint)));
		}
	}

	bool bounding_box::intersects(const vec3& org, const vec3& dir)
	{
		if (_updated)
		{
			mat4 modelTranslation = translate(_parent->getPosition());
			mat4 model = modelTranslation * _parent->getRotation();

			mapVertices();

			for (uint32_t i = 0; i < 8; i++)
			{
				_vertices[i] = mat4MulVec3(model, _vertices[i]);
			}

			mapTriangles();
			_updated = false;
		}

		for (int32_t i = 0; i < 12; i++)
		{
			if (rayTriangleIntersect(org, dir, _triangles[i]))
				return true;
		}

		return false;
	}
}