#include "stdafx.h"
#include "util.h"
#include "text_renderer.h"

TextRenderer::TextRenderer(GLuint textShader) : textShader(textShader)
{
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

void TextRenderer::genFontTexture(std::string &fontDir)
{
	std::string fontName = getFileName(fontDir);
	lowercase(fontName);
	Json::Value cachedFont = ResourceManager::loadConfig("resource/fonts/info/" + fontName);
	if(!verifyCache(cachedFont) || !genFontFromAtlas(fontDir, cachedFont)) genFontFromGlyphs(fontDir);
	//if(!genFontFromAtlas(fontDir)) genFontFromGlyphs(fontDir);
	//genFontFromGlyphs(fontDir);
}

bool TextRenderer::verifyCache(Json::Value &cache)
{
	if(cache["fontInfo"].get("RANGE", 0).asFloat() != RANGE || 
		cache["fontInfo"].get("TEXT_DIM", 0).asInt() != TEXT_DIM) return false;
	for (GLubyte c = 32; c < 127; ++c)
	{
		std::string sc(1, (char)c);
		const Json::Value character = cache[sc];
		if(!character) return false;
		if(!character["advance"]) return false;
		if(!character["bearing.x"]) return false;
		if(!character["bearing.y"]) return false;
		if(!character["size.x"]) return false;
		if(!character["size.y"]) return false;
		if(!character["textScale"]) return false;
	}
	return true;
}

bool TextRenderer::genFontFromAtlas(std::string &fontDir, Json::Value &cachedFont)
{
	std::string fontName = getFileName(fontDir);
	lowercase(fontName);

	TextureOptions options = {GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR};
	std::string atlasPath = "resource/fonts/msdf/" + fontName + ".bmp";
	std::vector<GLuint> textures = ResourceManager::loadAtlas(atlasPath, TEXT_DIM, TEXT_DIM, options, 95);
	if(textures.empty()) return false;
	for(int i = 0; i < textures.size(); ++i)
	{
		GLchar c = 32 + i;
		std::string sc(1, c);
		Character character = {
			textures[i],
			cachedFont[sc].get("textScale", 0).asFloat(),
			glm::vec2(cachedFont[sc].get("size.x", 0).asFloat(), cachedFont[sc].get("size.y", 0).asFloat()),
			glm::vec2(cachedFont[sc].get("bearing.x", 0).asFloat(), cachedFont[sc].get("bearing.y", 0).asFloat()),
			cachedFont[sc].get("advance", 0).asDouble()
		};
		fontTextures[fontName].insert(std::pair<GLchar, Character>(c, character));
	}
	return true;
}

void TextRenderer::genFontFromGlyphs(std::string &fontDir)
{
	std::string fontName = getFileName(fontDir);
	lowercase(fontName);
	FreetypeHandle *ft = initializeFreetype();
	std::vector< std::vector<unsigned char> > fontData;
	Json::Value dataCache(Json::objectValue);
	Json::Value fontInfo(Json::objectValue);
	fontInfo["TEXT_DIM"] = TEXT_DIM;
	fontInfo["RANGE"] = RANGE;
	dataCache["fontInfo"] = fontInfo;
	if (ft)
	{
		FontHandle *font = loadFont(ft, fontDir.c_str());
		if (font)
		{
			Shape shape;
			double scale = 1.0;
			getFontScale(scale, font);
			GLfloat textScale = TEXT_DIM/scale * 0.8;
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
				/// (&shape, max. angle)
				edgeColoringSimple(shape, 1.0);
				/// (width, height)
				Bitmap<FloatRGB> msdf(TEXT_DIM, TEXT_DIM);
				/// Auto frame the glyph
				double left = 0, bottom = 0, right = 0, top = 0;
				shape.bounds(left, bottom, right, top);
				Vector2 frame(TEXT_DIM, TEXT_DIM);
				Vector2 dims(right-left, top-bottom);
				Vector2 translate = 0.5*(frame/Vector2(scale, scale)) - Vector2(left, bottom);
				///(range, scale, translation)
				generateMSDF(msdf, shape, RANGE, textScale, translate);

				/// Convert font to ubytes
				std::vector<unsigned char> pixels(3*msdf.width()*msdf.height());
				std::vector<unsigned char>::iterator it = pixels.begin();
				for (int y = msdf.height()-1; y >= 0; --y)
					for (int x = 0; x < msdf.width(); ++x) 
					{
						*it++ = clamp(int(msdf(x, y).r*0x100), 0xff);
						*it++ = clamp(int(msdf(x, y).g*0x100), 0xff);
						*it++ = clamp(int(msdf(x, y).b*0x100), 0xff);
					}
				fontData.push_back(pixels);

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
				Character character = {
					texture,
					textScale,
					glm::vec2(right * textScale, top * textScale),
					glm::vec2(translate.x * textScale, dims.y * textScale),
					advance * textScale
				};
				std::string cs(1, (char)c);
				dataCache[cs] = charToJson(character);
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
	
	ResourceManager::saveAtlas("resource/fonts/msdf/" + fontName, TEXT_DIM, TEXT_DIM, COMP_RGB, fontData);
	ResourceManager::saveConfig("resource/fonts/info/" + fontName, dataCache);
}

Json::Value TextRenderer::charToJson(Character character)
{
	Json::Value characterValue(Json::objectValue);
	characterValue["textScale"] = character.textScale;
	characterValue["size.x"] = character.size.x;
	characterValue["size.y"] = character.size.y;
	characterValue["bearing.x"] = character.bearing.x;
	characterValue["bearing.y"] = character.bearing.y;
	characterValue["advance"] = character.advance;
	return(characterValue);
}

void TextRenderer::renderText(std::string fontDir, std::string text, GLfloat x, 
GLfloat y, GLfloat scale, glm::vec3 color, TextBorder border, glm::vec3 borderColor)
{
	if(text.empty()) return;
	scale = scale / TEXT_DIM;
	std::string fontName = getFileName(fontDir);
	lowercase(fontName);
	if (fontTextures.count(fontName) == 0)
		genFontTexture(fontDir);
	glEnable(GL_BLEND);
	glUseProgram(textShader);
	glUniform3f(glGetUniformLocation(textShader, "textColor"), color.r, color.g, color.b);
	glUniform3f(glGetUniformLocation(textShader, "borderColor"), borderColor.r, borderColor.g, borderColor.b);
	glUniform1f(glGetUniformLocation(textShader, "borderSize"), border);
	glUniform1f(glGetUniformLocation(textShader, "dimScale"), scale);
	glUniform1f(glGetUniformLocation(textShader, "fontScale"), TEXT_SCALE / fontTextures[fontName][32].textScale * 0.8);
	glUniform1f(glGetUniformLocation(textShader, "range"), RANGE);
	
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	
	GLfloat w = TEXT_DIM * scale;
	GLfloat h = TEXT_DIM * scale;
	/// Need to shift by first character to left align text
	GLfloat leftShift = fontTextures[fontName][text.at(0)].bearing.x * scale;
	for (auto c = text.begin(); c != text.end(); ++c)
	{
		Character ch = fontTextures[fontName][*c];
		//GLfloat xpos = (x - a) + (ch.bearing.x * scale);
		GLfloat xpos = x - leftShift;
		GLfloat ypos = y + (ch.size.y - ch.bearing.y) * scale;

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

		x += ch.advance * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}