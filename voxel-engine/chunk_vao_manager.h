#pragma once
#include "stdafx.h"
#include "chunk.h"
#include "util.h"

class ChunkVaoManager
{
private:
	ChunkVao getChunkVao();

	std::vector<ChunkVao> vaoPool;
	std::map<GLuint, ChunkVao> chunkVaos;
public:
	ChunkVaoManager();
	~ChunkVaoManager();
	void addChunkMesh(ChunkMesh *chunkMesh);
	void deleteChunkMesh(ChunkMesh *chunkMesh);
	void drawChunks();
	void initVaoPool();
};