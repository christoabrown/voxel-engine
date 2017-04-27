#pragma once
#include "stdafx.h"
#include "lib/msdfgen/msdfgen.h"
#include "lib/msdfgen/msdfgen-ext.h"

using namespace msdfgen;
class TextRenderer
{
private:
	const GLfloat TEXT_SCALE = 2.0;
	const int TEXT_DIM = 32 * TEXT_SCALE;
	GLuint textShader;
	GLuint VAO, VBO;
	struct Character {
		GLuint textureID;
		glm::ivec2 size; ///size of glyph
		glm::ivec2 bearing; ///offset from baseline to left/top of glyph
		GLuint advance; ///offset to advance to next glyph
	};
	std::map<std::string, std::map <GLchar, Character>> fontTextures;
	void genFontTexture(std::string fontDir);
public:
	TextRenderer(GLuint textShader);
	void renderText(std::string fontDir, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
};