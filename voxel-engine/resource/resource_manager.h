#pragma once
#include "../stdafx.h"
#include "../lib/jsoncpp/json/json.h"

#define COMP_Y 1
#define COMP_YA 2
#define COMP_RGB 3
#define COMP_RGBA 4

struct TextureOptions
{
    GLint wrapS = GL_REPEAT;
    GLint wrapT = GL_REPEAT;
    GLint minFilter = GL_LINEAR;
    GLint magFilter = GL_LINEAR;
};

//Singleton class
class ResourceManager
{
private:
	ResourceManager();
	~ResourceManager();
	/// Delete these two se we don't duplicate
	ResourceManager(ResourceManager const&);	
	void operator=(ResourceManager const&);
public:
	static ResourceManager& getInstance();

private:
    std::map<std::string, GLuint> textures;
    std::map<std::string, std::vector<GLuint> > textureAtlas;

    GLuint _gpuTexture(TextureOptions &options, int w, int h, void *data);
    GLuint _loadTexture(std::string &filePath, TextureOptions &options);
    int _saveBMP(std::string &savePath, int w, int h, int comp, const void *data);
    int _saveAtlas(std::string &savePath, int w, int h, int comp, std::vector< std::vector<unsigned char> > &data);
    std::vector<GLuint> _loadAtlas(std::string &filePath, int w, int h, TextureOptions &options, int count);
    bool _saveConfig(std::string &savePath, Json::Value json);
    Json::Value _loadConfig(std::string &filePath);
public:
    static GLuint loadTexture(std::string filePath, TextureOptions& options);
    static int saveBMP(std::string savePath, int w, int h, int comp, const void *data);
    /// W and H are dimensions of single texture
    /// ordered by col->row
    static int saveAtlas(std::string savePath, int w, int h, int comp, std::vector< std::vector<unsigned char> > &data);
    static std::vector<GLuint> loadAtlas(std::string filePath, int w, int h, TextureOptions options, int count = 0);
    static bool saveConfig(std::string savePath, Json::Value json);
    static Json::Value loadConfig(std::string filePath);
};