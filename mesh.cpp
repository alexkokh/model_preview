#include "mesh.h"

namespace SE_CORE
{
	mesh::mesh()
	{
	}

	mesh::~mesh()
	{
	}

	void mesh::draw()
	{
		// bind vertex array and buffers, set vertex attribs
		render_target::draw();

		for (uint32_t i = 0; i < _MeshDrawRange.size(); i++)
		{
			vector<vector<draw_range_t>>& ranges = _MeshDrawRange[i];
			for (uint32_t j = 0; j < ranges.size(); j++)
			{
				vector<draw_range_t> range = ranges[j];
				for (uint32_t k = 0; k < range.size(); k++)
				{
					int err;
					glDrawElements(GL_TRIANGLES, range[k].count, GL_UNSIGNED_INT, (void *)(range[k].offset * sizeof(GLuint)));
					err = glGetError();
				}
			}
		}
	}

	int mesh::load(char *path)
	{
		if (!_initialized)
			return 0;

		dae_reader_t *reader = createDAEReader(path);
		int indices_size = 0;
		int vertices_size = 0;

		if (!reader)
			return -1;

		for (uint32_t i = 0; i < reader->geometry.size(); i++)
		{
			geometry_t &geometry = reader->geometry[i];
			indices_size += geometry.indices.size() * sizeof(GLuint);
			vertices_size += geometry.bufferData.size() * sizeof(float);
			_MeshDrawRange.push_back(geometry.meshes);
		}

		int err;

		glBindVertexArray(_VertexArrayID);

		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBufferData(GL_ARRAY_BUFFER, vertices_size, NULL, GL_STATIC_DRAW);
		err = glGetError();

		int offset = 0;
		for (uint32_t i = 0; i < reader->geometry.size(); i++)
		{
			geometry_t &geometry = reader->geometry[i];
			int data_size = geometry.bufferData.size() * sizeof(float);
			glBufferSubData(GL_ARRAY_BUFFER, offset, data_size, geometry.bufferData.data());
			err = glGetError();
			offset += data_size;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _BufferIDs[ElementBuffer]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, NULL, GL_STATIC_DRAW);
		err = glGetError();

		offset = 0;
		for (uint32_t i = 0; i < reader->geometry.size(); i++)
		{
			geometry_t &geometry = reader->geometry[i];
			int data_size = geometry.indices.size() * sizeof(GLuint);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, data_size, geometry.indices.data());
			err = glGetError();
			offset += data_size;
		}

		_ElementSize = getElementSize(reader);
		const int stride = _ElementSize * sizeof(float);

		vertex_attrib_t coords = { 0, 3, GL_FLOAT, GL_FALSE, stride, 0 };
		vertex_attrib_t normal = { 1, 3, GL_FLOAT, GL_FALSE, stride, (void *)12 };
		vertex_attrib_t texcoord = { 2, 2, GL_FLOAT, GL_FALSE, stride, (void *)24 };

		_vertexAttrib.push_back(coords);
		_vertexAttrib.push_back(normal);
		_vertexAttrib.push_back(texcoord);

		destroyDAEReader(reader);

		return 1;
	}
}