#pragma once

#include "common.h"
#include "rendering.h"
#include "billboard.h"

namespace SE_CORE
{
#define DEFAULT_FONT_IMAGE_X_OFFSET 8
#define DEFAULT_FONT_IMAGE_Y_OFFSET 8
#define DEFAULT_FONT_IMAGE_ALIGNMENT 4
#define DEFAULT_FONT_BACKGROUND_COLOR {255,0,255}

	typedef struct font_texture_s {
		int32_t numComponents;
		int32_t width;
		int32_t height;
		int32_t stride;
		int32_t characterHeight;
		uint8_t *pixels;
	} font_texture_t;

	class font : public billboard
	{
	private:
		const string _alphabet = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~";
		vector<int32_t> _offsets;
		vector<int32_t> _widths;
		string _text;

		font_texture_t* getFontTexture(char *imgPath, int32_t xOffset, int32_t yOffset, uint8_t bgColor[3]);
		void freeFontTexture(font_texture_t *texture);
		int32_t getBufferSize(font_texture_t *texture, int32_t xOffset, int32_t yOffset, uint8_t bgColor[3], vector<int32_t> &charactersWidth, vector<int32_t> &charactersOffset, int32_t &totalWidth);
		void buildVertexBuffer(string text);
	protected:
		GLuint _textures[1];

		void loadFontTexture(char *imgPath, int32_t xOffset, int32_t yOffset, uint8_t bgColor[3]);

	public:
		font(string name, char *imgPath, GLuint screenWidth, GLuint screenHeight);
		virtual ~font();
		virtual void render();
		void setText(char *text);
		void setText(char *text, vec2 pos);
		void setPos(vec2 pos);
	};
}