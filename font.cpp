#include "font.h"

namespace SE_CORE
{
	font::font(string name, char *imgPath, GLuint screenWidth, GLuint screenHeight) : billboard(name)
	{
		_vertexBuffer = NULL;
		_indexBuffer = NULL;
		uint8_t bgColor[] = DEFAULT_FONT_BACKGROUND_COLOR;
		_text = "";
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;
		loadFontTexture(imgPath, DEFAULT_FONT_IMAGE_X_OFFSET, DEFAULT_FONT_IMAGE_Y_OFFSET, bgColor);
		init();

		GLint sampler_loc = glGetUniformLocation(_program, "tex0");
		glUniform1i(sampler_loc, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	font::~font()
	{
	}

	void font::buildVertexBuffer(string text)
	{
		GLfloat xOffset = 0;

		const uint32_t length = text.length();
		const uint32_t numVertices = 4 * length;
		uint32_t vBufSize = numVertices * _elementSize * sizeof(GLfloat);
		uint32_t alphabetLength = _alphabet.length();
		uint32_t alphabetWidth = _offsets[alphabetLength - 1] + _widths[alphabetLength - 1];
		const uint32_t numIndices = length * 6;
		const uint32_t iBufSize = numIndices * sizeof(GLint);

		if (!_vertexBuffer || text.length() > _text.length())
		{
			free(_vertexBuffer);
			free(_indexBuffer);
			_vertexBuffer = (GLfloat *)malloc(vBufSize);
			_indexBuffer = (GLuint *)malloc(iBufSize);
		}
		
		for (uint32_t i = 0, n = 0; i < length; i++)
		{
			int32_t id = _alphabet.find(text[i]);

			GLfloat x0 = (GLfloat)_offsets[id] / alphabetWidth;
			GLfloat x1 = (GLfloat)(_offsets[id] + _widths[id]) / alphabetWidth;

			_vertexBuffer[n++] = xOffset;
			_vertexBuffer[n++] = 0;
			_vertexBuffer[n++] = 0;
			_vertexBuffer[n++] = x0; // s
			_vertexBuffer[n++] = 0; // t

						   // 1
			_vertexBuffer[n++] = xOffset;
			_vertexBuffer[n++] = _height;
			_vertexBuffer[n++] = 0;
			_vertexBuffer[n++] = x0; // s
			_vertexBuffer[n++] = 1; // t

						   // 2
			_vertexBuffer[n++] = xOffset + _widths[id];
			_vertexBuffer[n++] = _height;
			_vertexBuffer[n++] = 0;
			_vertexBuffer[n++] = x1; // s
			_vertexBuffer[n++] = 1; // t

						   // 3
			_vertexBuffer[n++] = xOffset + _widths[id];
			_vertexBuffer[n++] = 0;
			_vertexBuffer[n++] = 0;
			_vertexBuffer[n++] = x1; // s
			_vertexBuffer[n++] = 0; // t

			xOffset += _widths[id];
		}

		glBindVertexArray(_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, _BufferIDs[ArrayBuffer]);
		glBufferData(GL_ARRAY_BUFFER, vBufSize, _vertexBuffer, GL_DYNAMIC_DRAW);

		for (uint32_t i = 0, k = 0; i < length; i++)
		{
			uint32_t ind_offset = i * 4;
			_indexBuffer[k++] = ind_offset + 0;
			_indexBuffer[k++] = ind_offset + 1;
			_indexBuffer[k++] = ind_offset + 3;
			_indexBuffer[k++] = ind_offset + 1;
			_indexBuffer[k++] = ind_offset + 2;
			_indexBuffer[k++] = ind_offset + 3;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _BufferIDs[ElementBuffer]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iBufSize, _indexBuffer, GL_DYNAMIC_DRAW);
	}

	void font::setText(char *strText)
	{
		string text = string(strText);
		if (text != _text)
		{
			buildVertexBuffer(text);
			_text = text;
		}
	}

	void font::setText(char *strText, vec2 pos)
	{
		setPos(pos);
		setText(strText);
	}

	void font::setPos(vec2 pos)
	{
		_pos = pos;
	}

	//void font::render(char *str, GLfloat x, GLfloat y)
	void font::render()
	{
		billboard::prepareForRendering(_pos);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _textures[0]);
		/*
		string text = string(str);
		if (text != _text)
		{
			buildVertexBuffer(text);
			_text = text;
		} */

		GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
		if (depthTestEnabled)
			glDisable(GL_DEPTH_TEST);

		const int32_t length = _text.length();
		glDrawElements(GL_TRIANGLES, length*6, GL_UNSIGNED_INT, 0);

		if (depthTestEnabled)
			glEnable(GL_DEPTH_TEST);



		glDisable(GL_BLEND);
	}

	font_texture_t* font::getFontTexture(char *imgPath, int32_t xOffset, int32_t yOffset, uint8_t bgColor[3])
	{
		font_texture_t *texture = NULL;

		FILE *f;
		fopen_s(&f, imgPath, "rb");
		if (!f)
			return NULL;

		texture = (font_texture_t *)malloc(sizeof(font_texture_t));
		memset(texture, 0, sizeof(font_texture_t));

		BITMAPFILEHEADER bh;
		BITMAPINFOHEADER bih;

		fread(&bh, sizeof(bh), 1, f);
		fread(&bih, sizeof(bih), 1, f);

		const int32_t imgWidth = bih.biWidth;
		const int32_t imgHeight = bih.biHeight;
		const int32_t numComponents = bih.biBitCount / 8;
		const int32_t lineSize = imgWidth * numComponents;
		const int32_t stride = (lineSize + DEFAULT_FONT_IMAGE_ALIGNMENT - 1) & (0xFFFFFFFF ^ (DEFAULT_FONT_IMAGE_ALIGNMENT-1));
		const int32_t dataSize = stride*imgHeight;

		uint8_t *pixels = (uint8_t *)malloc(dataSize);
		fread(pixels, dataSize, 1, f);

		const int32_t startOffset = stride*DEFAULT_FONT_IMAGE_Y_OFFSET + DEFAULT_FONT_IMAGE_X_OFFSET*numComponents;
		uint8_t *buf = pixels + startOffset;

		for (int32_t i = 0; i < imgHeight; i++)
		{
			if (!memcmp(buf, bgColor, 3))
			{
				texture->characterHeight = i;
				break;
			}

			buf += stride;
		}

		texture->numComponents = numComponents;
		texture->width = imgWidth;
		texture->height = imgHeight;
		texture->stride = stride;
		texture->pixels = pixels;

		return texture;
	}

	void font::freeFontTexture(font_texture_t *texture)
	{
		if (texture != NULL)
		{
			if (texture->pixels != NULL)
				free(texture->pixels);

			free(texture);
		}
	}

	int32_t font::getBufferSize(font_texture_t *texture, int32_t xOffset, int32_t yOffset, uint8_t bgColor[3], vector<int32_t> &charactersWidth, vector<int32_t> &charactersOffset, int32_t &totalWidth)
	{
		int32_t bufferSize = 0;

		const int32_t stride = texture->stride;
		const int32_t numComponents = texture->numComponents;
		const int32_t startOffset = stride*DEFAULT_FONT_IMAGE_Y_OFFSET + DEFAULT_FONT_IMAGE_X_OFFSET*numComponents;
		const int32_t skip_offset = (DEFAULT_FONT_IMAGE_Y_OFFSET + texture->characterHeight)*stride;
		const int32_t characterRows = (texture->height - DEFAULT_FONT_IMAGE_Y_OFFSET) / (DEFAULT_FONT_IMAGE_Y_OFFSET + texture->characterHeight);
		uint8_t *buf = texture->pixels + stride*texture->height - (DEFAULT_FONT_IMAGE_Y_OFFSET+1)*stride;

		totalWidth = 0;

		for (int32_t i = 0; i < characterRows; i++)
		{
			bool prevIsBG = false;
			int32_t characterWidth = 0;
			for (int32_t j = DEFAULT_FONT_IMAGE_X_OFFSET*numComponents; j < stride; j += numComponents)
			{
				if (memcmp(&buf[j], bgColor, 3)) 
				{
					bufferSize++;
					characterWidth++;
					prevIsBG = false;
				}
				else 
				{
					if (!prevIsBG)
					{
						const int32_t offset = &buf[j] - texture->pixels - characterWidth*numComponents;// -(texture->characterHeight - 1)*stride;
						charactersOffset.push_back(offset);
						charactersWidth.push_back(characterWidth);
						totalWidth += characterWidth;
						characterWidth = 0;
					}

					prevIsBG = true;
				}
			}

			buf -= skip_offset;
		}

		bufferSize *= texture->characterHeight*numComponents;
		 
		return bufferSize;
	}

	void font::loadFontTexture(char *imgPath, int32_t xOffset, int32_t yOffset, uint8_t bgColor[3])
	{
		font_texture_t *texture = getFontTexture(imgPath, xOffset, yOffset, bgColor);
		vector<int32_t> charactersOffset;
		int32_t totalWidth;
		const int32_t characterHeight = texture->characterHeight;
		const int32_t bufferSize = getBufferSize(texture, xOffset, yOffset, bgColor, _widths, charactersOffset, totalWidth);
		const int32_t numComponents = texture->numComponents;
		const int32_t srcStride = texture->stride;
		const int32_t dstStride = totalWidth*numComponents;

		uint8_t *fontTexture = (uint8_t *)malloc(bufferSize);
		int32_t curCharacterOffset = 0;

		for (int32_t i = 0; i < (int32_t)charactersOffset.size(); i++) 
		{
			uint8_t *src = texture->pixels + charactersOffset[i];
			uint8_t *dst = fontTexture + curCharacterOffset;
			for (int32_t j = 0; j < characterHeight; j++)
			{
				memcpy(dst, src, _widths[i] * numComponents);
				src -= srcStride;
				dst += dstStride;
			}

			curCharacterOffset += _widths[i] * numComponents;
		}

#if defined(DEBUG) || defined(_DEBUG)
		BITMAPFILEHEADER bh = { 0 };
		BITMAPINFOHEADER bih = { 0 };

		bh.bfType = 19778;
		bh.bfOffBits = sizeof(bh) + sizeof(bih);
		bh.bfSize = bh.bfOffBits + bufferSize;

		bih.biSize = sizeof(bih);
		bih.biBitCount = numComponents * 8;
		bih.biWidth = totalWidth;
		bih.biHeight = characterHeight;
		bih.biPlanes = 1;
		
		uint8_t *dstTexture = (uint8_t *)malloc(bufferSize);
		uint8_t *src = fontTexture;
		uint8_t *dst = dstTexture + bufferSize - dstStride;

		for (int32_t i = 0; i < characterHeight; i++)
		{
			memcpy(dst, src, dstStride);
			src += dstStride;
			dst -= dstStride;
		}

		FILE *f;
		fopen_s(&f, "texture.bmp", "wb");
		if (f)
		{
			fwrite(&bh, sizeof(bh), 1, f);
			fwrite(&bih, sizeof(bih), 1, f);
			fwrite(dstTexture, bufferSize, 1, f);
			fclose(f);
		}

		free(dstTexture);
#endif

		_width = (GLfloat)totalWidth;
		_height = (GLfloat)characterHeight;
		_elementSize = 5;

		glGenTextures(1, _textures);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _textures[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, totalWidth, characterHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, fontTexture);

		free(fontTexture);

		uint32_t offset = 0;
		for (uint32_t i = 0; i < charactersOffset.size(); i++)
		{
			_offsets.push_back(offset);
			offset += _widths[i];
		}

		freeFontTexture(texture);
	}
}