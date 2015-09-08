#include "rendering.h"

namespace SE_CORE
{
	render_target::render_target()
	{
	}

	render_target::~render_target()
	{
		glDeleteVertexArrays(1, &m_VertexArrayID);
		glDeleteBuffers(NumBuffers, m_BufferIDs);
	}

	void render_target::draw()
	{
		for (uint32_t i = 0; i < m_MeshDrawRange.size(); i++)
		{
			vector<vector<draw_range_t>>& ranges = m_MeshDrawRange[i];
			for (uint32_t j = 0; j < ranges.size(); j++)
			{
				vector<draw_range_t> range = ranges[j];
				for (uint32_t k = 0; k < range.size(); k++)
				{
					int err;
					glDrawElements(GL_TRIANGLES, range[k].count, GL_UNSIGNED_INT, (void *)(range[k].offset * sizeof(GLuint)));
					err = glGetError();
					err = err;
				}
			}
		}
	}

	int render_target::load(char *path)
	{
		if (!init_gl_buffers())
			return 0;

		dae_reader_t *reader = createDAEReader(path);
		int indices_size = 0;
		int vertices_size = 0;

		if (!reader)
			return -1;

		for (uint32_t i = 0; i < reader->geometry.size(); i++)
		{
			geometry_t &geometry = reader->geometry[i];
			indices_size  += geometry.indices.size() * sizeof(GLuint);
			vertices_size += geometry.bufferData.size() * sizeof(float);
			m_MeshDrawRange.push_back(geometry.meshes);
		}

		int err;

		glBindVertexArray(m_VertexArrayID);

		glBindBuffer(GL_ARRAY_BUFFER, m_BufferIDs[ArrayBuffer]);
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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferIDs[ElementBuffer]);
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

		int stride = getElementSize(reader) * sizeof(float);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)12);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)24);
		glEnableVertexAttribArray(2);

		destroyDAEReader(reader);

		return 1;
	}

	int render_target::init_gl_buffers()
	{
		glGenVertexArrays(1, &m_VertexArrayID);
		if (!m_VertexArrayID)
			return glGetError();

		glGenBuffers(NumBuffers, m_BufferIDs);
		for (uint32_t i = 0; i < NumBuffers; i++)
		{
			if (!m_BufferIDs[i])
				return glGetError();
		}

		return 1;
	}
}