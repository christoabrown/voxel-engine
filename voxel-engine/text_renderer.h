#pragma once
#include "stdafx.h"
#include "lib/msdfgen/msdfgen.h"
#include "lib/msdfgen/msdfgen-ext.h"
#include "resource/resource_manager.h"

enum TextBorder {
	BORDER_NONE = 0,
	BORDER_THIN = 2,
	BORDER = 4,
	BORDER_THICK = 8
};

using namespace msdfgen;
class TextRenderer
{
private:
	const GLfloat TEXT_SCALE = 2.0;
	const int TEXT_DIM = 32 * TEXT_SCALE;
	/// Leave this at 4.0
	const GLfloat RANGE = 4.0;
	GLuint textShader;
	GLuint VAO, VBO;
	struct Character {
		GLuint textureID;
		GLfloat textScale;
		glm::vec2 size; ///size of glyph
		glm::vec2 bearing; ///offset from baseline to left/top of glyph
		double advance; ///offset to advance to next glyph
	};
	std::map<std::string, std::map <GLchar, Character>> fontTextures;
	void genFontTexture(std::string &fontDir);
	void genFontFromGlyphs(std::string &fontDir);
	bool genFontFromAtlas(std::string &fontDir, Json::Value &cachedFont);
	Json::Value charToJson(Character character);
	bool verifyCache(Json::Value &cache);
public:
	TextRenderer(GLuint textShader);
	void renderText(std::string fontDir, std::string text, GLfloat x, 
		GLfloat y, GLfloat scale, glm::vec3 color, TextBorder border = BORDER_NONE, glm::vec3 borderColor = glm::vec3(0, 0, 0));
};