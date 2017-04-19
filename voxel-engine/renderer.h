#pragma once
#include "stdafx.h"
#include "engine_win32.h"
#include "util.h"
#include "vbo_arena.h"


struct VAO {
	VAO() {};
	VAO(GLuint vao, GLuint baseVbo, GLuint texture, VboArena instances) :
		vao(vao), baseVbo(baseVbo), texture(texture), instances(instances) {};
	GLuint vao, baseVbo, texture;
	VboArena instances;
	bool operator==(const VAO& other) const {
		return (this->vao == other.vao);
	}
};

void addChunkMeshQueue(std::shared_ptr<ChunkMesh> newChunk);
void removeChunkMeshQueue(std::shared_ptr<ChunkMesh> chunkMesh);
void setupGL(WinInfo *winInfo);
GLfloat DeltaTime();