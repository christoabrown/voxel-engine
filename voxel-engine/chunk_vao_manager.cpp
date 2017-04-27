#include "stdafx.h"
#include "chunk_vao_manager.h"
#include "renderer.h"
#include "chunk.h"
#include "camera.h"

ChunkVaoManager::ChunkVaoManager()
{
}

ChunkVaoManager::~ChunkVaoManager()
{
	//for (auto &vao : chunkVaos)
	//{
		//glDeleteBuffers(1, &vao.second.vbo);
		//glDeleteVertexArrays(1, &vao.second.vao);
	//}
}

void ChunkVaoManager::initVaoPool()
{
	const int COUNT = WORLD_EXPANSE;
	GLuint* vaos = new GLuint[COUNT];
	GLuint* vbos = new GLuint[COUNT];
	glGenVertexArrays(COUNT, vaos);
	glGenBuffers(COUNT, vbos);
	for (int i = 0; i < COUNT; ++i)
	{
		glBindVertexArray(vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
		// Position attribute;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, r)));
		glEnableVertexAttribArray(1);
		//texcoord attribute
		glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));
		glEnableVertexAttribArray(2);
		//normal attribute
		glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normX)));
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		vaoPool.push_back(ChunkVao{ vaos[i], vbos[i] });
	}
	delete[] vaos;
	delete[] vbos;
}

ChunkVao ChunkVaoManager::getChunkVao()
{
	if (!vaoPool.empty())
	{
		ChunkVao chunkVao = vaoPool.back();
		vaoPool.pop_back();
		return(chunkVao);
	}
	else
	{
		GLuint vao, vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// Position attribute;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, r)));
		glEnableVertexAttribArray(1);
		//texcoord attribute
		glVertexAttribPointer(2, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));
		glEnableVertexAttribArray(2);
		//normal attribute
		glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normX)));
		glEnableVertexAttribArray(3);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return(ChunkVao{ vao, vbo });
	}
}

void ChunkVaoManager::addChunkMesh(ChunkMesh *chunkMesh)
{
	ChunkVao chunkVao = getChunkVao();
	glBindBuffer(GL_ARRAY_BUFFER, chunkVao.vbo);
	glBufferData(GL_ARRAY_BUFFER, chunkMesh->meshData.size() * sizeof(Vertex), &chunkMesh->meshData[0], GL_DYNAMIC_DRAW);
	chunkVao.verticeCount = chunkMesh->meshData.size();
	chunkVao.chunkPos = glm::vec3(chunkMesh->meshData[0].x, chunkMesh->meshData[0].y, chunkMesh->meshData[0].z);
	chunkMesh->chunkVao = chunkVao;
	
	chunkVaos[chunkVao.vao] = chunkVao;
}

void ChunkVaoManager::deleteChunkMesh(ChunkMesh *chunkMesh)
{
	glBindBuffer(GL_ARRAY_BUFFER, chunkMesh->chunkVao.vbo);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	chunkVaos.erase(chunkMesh->chunkVao.vao);
	vaoPool.push_back(chunkMesh->chunkVao);
}

void ChunkVaoManager::drawChunks()
{
	for (auto &chunk : chunkVaos)
	{
		if (pointInFrustum(chunk.second.chunkPos))
		{
			glBindVertexArray(chunk.second.vao);
			glDrawArrays(GL_TRIANGLES, 0, chunk.second.verticeCount);
		}
	}
}