#pragma once
#include "stdafx.h"
typedef unsigned int BlockID;
#pragma pack(push, 1)
struct Vertex {
	GLfloat x, y, z;
	GLubyte r, g, b;
	GLubyte u, v;
	GLbyte normX, normY, normZ;
};
#pragma pack(pop)
struct ChunkVao
{
	ChunkVao() {};
	ChunkVao(GLuint vao, GLuint vbo) : vao(vao), vbo(vbo) {};
	GLuint vao, vbo;
	size_t verticeCount;
	glm::vec3 chunkPos;
};

struct ChunkMesh {
	//std::vector<glm::vec3> meshData;
	//std::vector<size_t> vboRegions;
	std::vector<Vertex> meshData;
	ChunkVao chunkVao;
};

struct DataBlock {
	size_t startIndex;
	size_t size;
	glm::vec3 startData;
};

struct Plane {
	Plane(void) : a(0), b(0), c(0), d(0) {};
	Plane(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2) {
		glm::vec3 n = glm::normalize(glm::cross(p1 - p0, p2 - p0));
		a = n.x;
		b = n.y;
		c = n.z;
		d = -glm::dot(p0, n);
	};
	GLfloat signedDistance(glm::vec3 &pt)
	{
		return (a * pt.x + b * pt.y + c * pt.z + d);
	}
	GLfloat a, b, c, d;
};

std::string getFileName(std::string& dir);
std::string getFileExt(std::string& dir);
void lowercase(std::string& str);

enum Face { plusX, minusX, plusY, minusY, plusZ, minusZ };