#include "resource_manager.h"
#include "../util.h"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb_image_write.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

ResourceManager& ResourceManager::getInstance()
{
    static ResourceManager instance;
    return(instance);
}

GLuint ResourceManager::_gpuTexture(TextureOptions &options, int w, int h, void *data)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, options.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, options.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options.magFilter);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    if(options.minFilter == GL_NEAREST_MIPMAP_NEAREST || options.minFilter == GL_LINEAR_MIPMAP_NEAREST
    || options.minFilter == GL_NEAREST_MIPMAP_LINEAR || options.minFilter == GL_LINEAR_MIPMAP_LINEAR)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return(texture);
}

GLuint ResourceManager::_loadTexture(std::string &filePath, TextureOptions& options)
{
    std::string fileName = getFileName(filePath);
    lowercase(fileName);
    /// Check if texture is already loaded
    if(textures.count(fileName))
    {
        return(textures[fileName]);
    }
    /// Otherwise load it into GPU
    GLFWwindow* window = glfwGetCurrentContext();
    if(!window)
    {
        std::cout << "ERROR::RESOURCE_MANAGER::Can't load a texture without context" << std::endl;
        return(0);
    }

    int w, h, bpp;
    GLubyte *image = stbi_load(filePath.c_str(), &w, &h, &bpp, 4);
    if(w <= 0 || h <= 0 || image == NULL) return(0);
    GLuint texture = _gpuTexture(options, w, h, image);
    stbi_image_free(image);
    
    textures[fileName] = texture;
    return(texture);
}
GLuint ResourceManager::loadTexture(std::string filePath, TextureOptions &options)
{return ResourceManager::getInstance()._loadTexture(filePath, options);}

int ResourceManager::_saveBMP(std::string &savePath, int w, int h, int comp, const void *data)
{
    if(getFileExt(savePath) != ".bmp") savePath += ".bmp";
    int result = stbi_write_bmp(savePath.c_str(), w, h, comp, data);
    if(!result)
    {
        std::cout << "ERROR::RESOURCE_MANAGER::Image save failed. " << result << std::endl;
    }
    return(result);
}
int ResourceManager::saveBMP(std::string savePath, int w, int h, int comp, const void *data)
{return ResourceManager::getInstance()._saveBMP(savePath, w, h, comp, data);}

int ResourceManager::_saveAtlas(std::string &savePath, int w, int h, int comp, std::vector< std::vector<unsigned char> > &data)
{
    double dim = sqrt(data.size());
    int countCol = dim + 1;
    int countRow = dim + 1;
    int atlasWidth = w*countCol;
    int atlasHeight = h*countRow;
    int atlasSize = comp*atlasWidth*atlasHeight;
    std::vector<unsigned char> atlas(atlasSize);
    for(int i = 0; i < data.size(); ++i)
    {
        std::vector<unsigned char>::iterator it = data[i].begin();
        int curCol = i % countCol;
        int curRow = i / countRow;
        /// Set up position for next texture
        int atlasPos = curCol*w*comp + curRow*atlasWidth*h*comp;
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x) 
            {
                atlas[atlasPos++] = *it++;
                atlas[atlasPos++] = *it++;
                atlas[atlasPos++] = *it++;
            }
            /// Advance one row minus width of texture
            atlasPos += atlasWidth*comp - w*comp;
        }
    }
    int result = _saveBMP(savePath, atlasWidth, atlasHeight, comp, &atlas[0]);
    if(!result)
    {
        std::cout << "ERROR::RESOURCE_MANAGER::Atlas save failed. " << result << std::endl;
    }
    return(result);
}
int ResourceManager::saveAtlas(std::string savePath, int w, int h, int comp, std::vector< std::vector<unsigned char> > &data)
{return ResourceManager::getInstance()._saveAtlas(savePath, w, h, comp, data);}

std::vector<GLuint> ResourceManager::_loadAtlas(std::string &filePath, int w, int h, TextureOptions &options, int count)
{
    std::string fileName = getFileName(filePath);
    lowercase(fileName);
    /// Check if texture is already loaded
    if(textureAtlas.count(fileName))
    {
        return(textureAtlas[fileName]);
    }
    /// Otherwise load it into GPU
    std::vector<GLuint> atlas;
    GLFWwindow* window = glfwGetCurrentContext();
    if(!window)
    {
        std::cout << "ERROR::RESOURCE_MANAGER::Can't load a texture without context" << std::endl;
        return(atlas);
    }

    int atlasWidth, atlasHeight, comp;
    GLubyte *image = stbi_load(filePath.c_str(), &atlasWidth, &atlasHeight, &comp, 4);
    if(atlasWidth <= 0 || atlasHeight <= 0 || image == NULL) return(atlas);
    comp = 4;
    //std::cout << atlasWidth << std::endl;
    //atlasHeight = atlasWidth;
    int countCol = atlasWidth/w;
    int countRow = atlasHeight/h;
    if(!count) count = (atlasWidth / w) * (atlasHeight / h);
    for(int i = 0; i < count; ++i)
    {
        std::vector<GLubyte> tex(w*h*comp);
        std::vector<GLubyte>::iterator it = tex.begin();
        int curCol = i % countCol;
        int curRow = i / countRow;
        /// Set up position for next texture
        /// Opengl wants this upside down so we add extra term
        int atlasPos = curCol*w*comp + curRow*atlasWidth*h*comp + atlasWidth*(h-1)*comp;
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x) 
                for(int c = 0; c < comp; ++c)
                    *it++ = image[atlasPos++];
            /// Advance one row minus width of texture
            atlasPos -= atlasWidth*comp + w*comp;
        }
        GLuint texture = _gpuTexture(options, w, h, &tex[0]);
        atlas.push_back(texture);
    }
    textureAtlas[fileName] = atlas;
    stbi_image_free(image);
    return(atlas);
}
std::vector<GLuint> ResourceManager::loadAtlas(std::string filePath, int w, int h, TextureOptions options, int count)
{return ResourceManager::getInstance()._loadAtlas(filePath, w, h, options, count);}

bool ResourceManager::_saveConfig(std::string &savePath, Json::Value json)
{
    if(getFileExt(savePath) != ".json") savePath += ".json";
    std::ofstream out(savePath);
    if(out.is_open())
        out << json;
    else
    {
        std::cout << "ERROR::RESOURCE_MANAGER::Couldnt open " << savePath << std::endl;
        return false;
    }
    out.close();
    return true;
}
bool ResourceManager::saveConfig(std::string savePath, Json::Value json)
{return ResourceManager::getInstance()._saveConfig(savePath, json);}

Json::Value ResourceManager::_loadConfig(std::string &filePath)
{
    if(getFileExt(filePath) != ".json") filePath += ".json";
    std::ifstream in(filePath);
    Json::Value value;
    if(in.is_open())
        in >> value;
    else
        std::cout << "ERROR::RESOURCE_MANAGER::Couldnt open " << filePath << std::endl;
    return(value);
}
Json::Value ResourceManager::loadConfig(std::string filePath)
{return ResourceManager::getInstance()._loadConfig(filePath);}