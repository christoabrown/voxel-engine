/*#pragma once
#include "stdafx.h"
#include "util.h"

//6,144 bytes default size
const size_t DEFAULT_VBOARENA_SIZE = 512;
typedef std::unordered_map<size_t, DataBlock> Region;
const glm::vec3 NULL_LOCATION = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);

class VboArena
{
private:
	GLuint vboID;
	size_t capacity, size;
	std::map<size_t, size_t> freeStarts;
	std::map<size_t, size_t> freeEnds;
	Region regions;

	DataBlock getFreeBlock();
	void mergeFreeBlock(DataBlock &block);
	void addFreeBlock(DataBlock &block);
	void resizeArena(size_t newSpace);
public:
	VboArena();
	void deleteData();
	//void addRegion(ChunkMeshInfo &meshInfo);
	//void freeRegion(ChunkMeshInfo &meshInfo);
	size_t getSize();
	void drawInstances();
};*/