#include "stdafx.h"
#include "util.h"
#include "textRenderer.h"

TextRenderer::TextRenderer(GLuint textShader) : textShader(textShader)
{
	//textShader = Shader("msdf_default.vert", "msdf_default.frag");
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextRenderer::genFontTexture(std::string fontDir)
{
	std::string fontName = getFileName(fontDir);
	std::transform(fontName.begin(), fontName.end(), fontName.begin(), ::tolower);
	FreetypeHandle *ft = initializeFreetype();
	if (ft)
	{
		FontHandle *font = loadFont(ft, fontDir.c_str());
		if (font)
		{
			Shape shape;
			/// Only need printable characters
			for (GLubyte c = 32; c < 127; ++c)
			{
				double advance = 1.0;
				if (!loadGlyph(shape, font, c, &advance))
				{
					std::cout << "ERROR::TEXTRENDER:: Failed to load Glyph " << (char)c << std::endl;
					continue;
				}
				shape.normalize();
				///(&shape, max. angle)
				edgeColoringSimple(shape, 1.0);
				///(width, height)
				Bitmap<FloatRGB> msdf(TEXT_DIM, TEXT_DIM);
				///(range, scale, translation)
				generateMSDF(msdf, shape, 4.0, TEXT_SCALE, Vector2(4.0, 8.0));
				
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D, ///target
					0, ///level
					GL_RGB, ///internal format
					TEXT_DIM, ///width
					TEXT_DIM, ///height
					0, ///border
					GL_RGB, ///format
					GL_FLOAT, ///type
					msdf.content
				);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				double left = 0, bottom = 0, right = 0, top = 0;
				shape.bounds(left, bottom, right, top);
				Character character = {
					texture,
					glm::ivec2((int)right, (int)top),
					glm::ivec2((int)(left), (int)(top - bottom)),
					(GLuint)advance
				};
				fontTextures[fontName].insert(std::pair<GLchar, Character>(c, character));
			}
			destroyFont(font);
		}
		else
			std::cout << "ERROR::TEXTRENDER:: failed to load font " << fontDir.c_str() << std::endl;
		deinitializeFreetype(ft);
	}
	else
		std::cout << "ERROR::TEXTRENDER:: failed to initialize free type " << std::endl;
}

void TextRenderer::renderText(std::string fontDir, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	std::string fontName = getFileName(fontDir);
	std::transform(fontName.begin(), fontName.end(), fontName.begin(), ::tolower);
	if (fontTextures.count(fontName) == 0)
		genFontTexture(fontDir);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glUseProgram(textShader);
	glUniform3f(glGetUniformLocation(textShader, "textColor"), color.r, color.g, color.b);
	glUniform1f(glGetUniformLocation(textShader, "scale"), scale);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); ++c)
	{
		Character ch = fontTextures[fontName][*c];
		GLfloat xpos = x + ch.bearing.x * scale;
		//GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
		GLfloat ypos = y;
		//GLfloat w = ch.size.x * scale;
		GLfloat w = TEXT_DIM * scale;
		//GLfloat h = ch.size.y * scale;
		GLfloat h = TEXT_DIM * scale;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos,     ypos,       0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 0.0 },

			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 },
			{ xpos + w, ypos + h,   1.0, 1.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += ch.advance * scale * TEXT_SCALE;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}