#include "mesh.h"

namespace SE_CORE
{
	mesh::mesh(string name, ShaderInfo *shaders, uint32_t numShaders) : scene_object(name, shaders, numShaders)
	{
	}

	mesh::~mesh()
	{
	}

	void mesh::render(camera *cam)
	{
		if (!_visible)
			return;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		scene_object::render(cam);

		for (uint32_t i = 0; i < _MeshDrawRange.size(); i++)
		{
			vector<vector<draw_range_t>>& ranges = _MeshDrawRange[i];
			for (uint32_t j = 0; j < ranges.size(); j++)
			{
				vector<draw_range_t> range = ranges[j];
				for (uint32_t k = 0; k < range.size(); k++)
				{
					glDrawElements(GL_TRIANGLES, range[k].count, GL_UNSIGNED_INT, (void *)(range[k].offset * sizeof(GLuint)));
				}
			}
		}

		if (_boundingVolume->getVisible())
		{
			_boundingVolume->setPosition(_position);
			_boundingVolume->setRotation(_rotation);
			_boundingVolume->render(cam);
		}

		glDisable(GL_BLEND);
	}

	uint32_t mesh::load(char *path)
	{
		if (!_buffersInitialized)
			return 0;

		dae_reader_t *reader = createDAEReader(path);
		uint32_t indices_size = 0;
		uint32_t vertices_size = 0;

		if (!reader)
			return -1;

		for (uint32_t i = 0; i < reader->geometry.size(); i++)
		{
			geometry_t &geometry = reader->geometry[i];
			indices_size += geometry.indices.size() * sizeof(GLuint);
			vertices_size += geometry.bufferData.size() * sizeof(float);
			_MeshDrawRange.push_back(geometry.meshes);
		}

		glBindVertexArray(_VertexArrayID);

		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBufferData(GL_ARRAY_BUFFER, vertices_size, NULL, GL_STATIC_DRAW);

		uint32_t offset = 0;
		for (uint32_t i = 0; i < reader->geometry.size(); i++)
		{
			geometry_t &geometry = reader->geometry[i];
			uint32_t data_size = geometry.bufferData.size() * sizeof(float);
			glBufferSubData(GL_ARRAY_BUFFER, offset, data_size, geometry.bufferData.data());
			offset += data_size;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _BufferIDs[ElementBuffer]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, NULL, GL_STATIC_DRAW);

		offset = 0;
		for (uint32_t i = 0; i < reader->geometry.size(); i++)
		{
			geometry_t &geometry = reader->geometry[i];
			uint32_t data_size = geometry.indices.size() * sizeof(GLuint);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, data_size, geometry.indices.data());
			offset += data_size;
		}

		_elementSize = getElementSize(reader);
		const int32_t stride = _elementSize * sizeof(float);

		vertex_attrib_t coords = { 0, 3, GL_FLOAT, GL_FALSE, stride, 0 };
		vertex_attrib_t normal = { 1, 3, GL_FLOAT, GL_FALSE, stride, (void *)12 };
		vertex_attrib_t texcoord = { 2, 2, GL_FLOAT, GL_FALSE, stride, (void *)24 };

		_vertexAttrib.push_back(coords);
		_vertexAttrib.push_back(normal);
		_vertexAttrib.push_back(texcoord);

		_boundingVolume = new bounding_box(this, reader, _name);

		destroyDAEReader(reader);

		return 1;
	}
}