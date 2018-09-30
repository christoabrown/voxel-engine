#pragma once
#include "stdafx.h"
#include "map3d.h"
#include "renderer.h"
#include "util.h"
#include "chunk_vao_manager.h"

class Chunk {
private:
	void meshCube(ChunkMesh &mesh, BlockID blockID, int &x, int &y, int &z);
	void getAOQuad(Face face, GLubyte *corners, int x, int y, int z);
	GLubyte getAOValue(bool side1, bool side2, bool corner);
public:
	Chunk(void) : data(nullptr), meshed(false), airBlocks(airBlocks), generated(false) {};
	Chunk(Point chunkPos, BYTE *data, int airBlocks) : chunkPos(chunkPos), data(data), airBlocks(airBlocks), meshed(false) {};
	void mesh();
	static bool isAir(BYTE block);

	Point chunkPos;
	BYTE *data = nullptr;
	Chunk *plusX = nullptr, *minusX = nullptr, *plusY = nullptr, *minusY = nullptr, *plusZ = nullptr, *minusZ = nullptr;
	int airBlocks;
	bool meshed;
	bool generated;
	std::shared_ptr<ChunkMesh> chunkMesh;
};

const int WORLD_SIZE = 20;
const int WORLD_SIZE_SQ = WORLD_SIZE*WORLD_SIZE;
const int CHUNK_SIZE = 32;
const int CHUNK_SIZE_SQ = CHUNK_SIZE*CHUNK_SIZE;
const int WORLD_EXPANSE = WORLD_SIZE*WORLD_SIZE*WORLD_SIZE;
const int CHUNK_EXPANSE = CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE;
//NOTE: Height scale changes how tall mountains are
const int HEIGHT_SCALE = 256;
//NOTE: sea level is where things start to get cut off from height map
const int SEA_LEVEL = 0;
//NOTE: higher air threshold = more empty space
const float AIR_THRESHOLD = -0.3f;