#include "stdafx.h"
#include "renderer.h"
#include "chunk.h"
#include "map3d.h"
#include "safe_queue.h"
#include "world.h"
#include "block.h"

#include "lib\fastnoise\FastNoiseSIMD.h"
void Chunk::mesh()
{
	if (airBlocks == CHUNK_EXPANSE || data == nullptr) return; //nothing to mesh
	chunkMesh = std::make_shared<ChunkMesh>();
	for (int i = 0; i < CHUNK_EXPANSE; ++i)
	{
		int blockX = i / CHUNK_SIZE_SQ;
		int b = i - CHUNK_SIZE_SQ * blockX;
		int blockY = b / CHUNK_SIZE;
		int blockZ = b % CHUNK_SIZE;
		if(isAir(data[i]))
			continue;
		meshCube(*chunkMesh, data[i], blockX, blockY, blockZ);
	}
	if(chunkMesh->meshData.size() > 0)
		addChunkMeshQueue(chunkMesh);
	meshed = true;
}

GLubyte Chunk::getAOValue(bool side1, bool side2, bool corner)
{
	const GLubyte NO_AO = 255;
	const GLubyte AO_MOD = 255 / 5;
	if (side1 && side2)
		return(NO_AO - 3 * AO_MOD);
	else
		return(NO_AO - (((GLubyte)side1 + (GLubyte)side2 + (GLubyte)corner) * AO_MOD));

}

void Chunk::getAOQuad(Face face, GLubyte *corners, int x, int y, int z)
{
	switch (face)
	{
	case Face::plusX : 
	{
		x += 1;
		bool minZ = World::blockAt(this, x, y, z - 1) != 0;	
		bool plY = World::blockAt(this, x, y + 1, z) != 0;	
		bool plZ = World::blockAt(this, x, y, z + 1) != 0;
		bool minY = World::blockAt(this, x, y - 1, z) != 0;
		bool minZplY = World::blockAt(this, x, y + 1, z - 1) != 0;
		bool plYplZ = World::blockAt(this, x, y + 1, z + 1) != 0;
		bool plZminY = World::blockAt(this, x, y - 1, z + 1) != 0;
		bool minYminZ = World::blockAt(this, x, y - 1, z - 1) != 0;
		corners[0] = corners[5] = getAOValue(plZ, minY, plZminY);
		corners[1] = getAOValue(minY, minZ, minYminZ);
		corners[2] = corners[3] = getAOValue(minZ, plY, minZplY);
		corners[4] = getAOValue(plZ, plY, plYplZ);
		break;
	}
	case Face::minusX :
	{
		x -= 1;
		bool plZ = World::blockAt(this, x, y, z + 1) != 0;
		bool minZ = World::blockAt(this, x, y, z - 1) != 0;
		bool plY = World::blockAt(this, x, y + 1, z) != 0;
		bool minY = World::blockAt(this, x, y - 1, z) != 0;
		bool minYminZ = World::blockAt(this, x, y - 1, z - 1) != 0;
		bool minYplZ = World::blockAt(this, x, y - 1, z + 1) != 0;
		bool plYplZ = World::blockAt(this, x, y + 1, z + 1) != 0;
		bool plYminZ = World::blockAt(this, x, y + 1, z - 1) != 0;
		corners[0] = corners[5] = getAOValue(minY, minZ, minYminZ);
		corners[1] = getAOValue(minY, plZ, minYplZ);
		corners[2] = corners[3] = getAOValue(plY, plZ, plYplZ);
		corners[4] = getAOValue(plY, minZ, plYminZ);
		break;
	}
	case Face::plusY :
	{
		y += 1;
		bool plZ = World::blockAt(this, x, y, z + 1) != 0;
		bool minZ = World::blockAt(this, x, y, z - 1) != 0;
		bool plX = World::blockAt(this, x + 1, y, z) != 0;
		bool minX = World::blockAt(this, x - 1, y, z) != 0;
		bool plZplX = World::blockAt(this, x + 1, y, z + 1) != 0;
		bool plXminZ = World::blockAt(this, x + 1, y, z - 1) != 0;
		bool minXminZ = World::blockAt(this, x - 1, y, z - 1) != 0;
		bool minXplZ = World::blockAt(this, x - 1, y, z + 1) != 0;
		corners[0] = corners[5] = getAOValue(plZ, plX, plZplX);
		corners[1] = getAOValue(plX, minZ, plXminZ);
		corners[2] = corners[3] = getAOValue(minX, minZ, minXminZ);
		corners[4] = getAOValue(minX, plZ, minXplZ);
		break;
	}
	case Face::minusY :
	{
		y -= 1;
		bool plZ = World::blockAt(this, x, y, z + 1) != 0;
		bool minZ = World::blockAt(this, x, y, z - 1) != 0;
		bool plX = World::blockAt(this, x + 1, y, z) != 0;
		bool minX = World::blockAt(this, x - 1, y, z) != 0;
		bool plZplX = World::blockAt(this, x + 1, y, z + 1) != 0;
		bool plXminZ = World::blockAt(this, x + 1, y, z - 1) != 0;
		bool minXminZ = World::blockAt(this, x - 1, y, z - 1) != 0;
		bool minXplZ = World::blockAt(this, x - 1, y, z + 1) != 0;
		corners[0] = corners[5] = getAOValue(minX, minZ, minXminZ);
		corners[1] = getAOValue(plX, minZ, plXminZ);
		corners[2] = corners[3] = getAOValue(plZ, plX, plZplX);
		corners[4] = getAOValue(minX, plZ, minXplZ);
		break;
	}
	case Face::plusZ :
	{
		z += 1;
		bool plY = World::blockAt(this, x, y + 1, z) != 0;
		bool minY = World::blockAt(this, x, y - 1, z) != 0;
		bool plX = World::blockAt(this, x + 1, y, z) != 0;
		bool minX = World::blockAt(this, x - 1, y, z) != 0;
		bool minYminX = World::blockAt(this, x - 1, y - 1, z) != 0;
		bool plXminY = World::blockAt(this, x + 1, y - 1, z) != 0;
		bool plYplX = World::blockAt(this, x + 1, y + 1, z) != 0;
		bool minXplY = World::blockAt(this, x - 1, y + 1, z) != 0;
		corners[0] = corners[5] = getAOValue(minY, minX, minYminX);
		corners[1] = getAOValue(plX, minY, plXminY);
		corners[2] = corners[3] = getAOValue(plY, plX, plYplX);
		corners[4] = getAOValue(minX, plY, minXplY);
		break;
	}
	case Face::minusZ :
	{
		z -= 1;
		bool plY = World::blockAt(this, x, y + 1, z) != 0;
		bool minY = World::blockAt(this, x, y - 1, z) != 0;
		bool plX = World::blockAt(this, x + 1, y, z) != 0;
		bool minX = World::blockAt(this, x - 1, y, z) != 0;
		bool minYminX = World::blockAt(this, x - 1, y - 1, z) != 0;
		bool plXminY = World::blockAt(this, x + 1, y - 1, z) != 0;
		bool plYplX = World::blockAt(this, x + 1, y + 1, z) != 0;
		bool minXplY = World::blockAt(this, x - 1, y + 1, z) != 0;
		corners[0] = corners[5] = getAOValue(minY, minX, minYminX);
		corners[1] = getAOValue(minX, plY, minXplY);
		corners[2] = corners[3] = getAOValue(plY, plX, plYplX);
		corners[4] = getAOValue(plX, minY, plXminY);
		break;
	}
	default :
		corners[0] = corners[1] = corners[2] = corners[3] = corners[4] = corners[5] = 255;
	}
}

void Chunk::meshCube(ChunkMesh &inst, BlockID blockID, int &x, int &y, int &z)
{
	int worldX = chunkPos.x * CHUNK_SIZE + x;
	int worldY = chunkPos.y * CHUNK_SIZE + y;
	int worldZ = chunkPos.z * CHUNK_SIZE + z;
	//auto *block = &inst[blockID];
	for(int i = 0; i <= Face::minusZ; ++i)
	{
		const Point *direction = &unitDirection[i];
		int adjX = x + direction->x;
		int adjY = y + direction->y;
		int adjZ = z + direction->z;
		BYTE adjBlock = World::blockAt(this, adjX, adjY, adjZ);
		if(adjBlock == 0)
		{	
			GLubyte aoQuad[6];
			getAOQuad(Face(i), aoQuad, x, y, z);
			int j = 0;
			for (auto vert : baseCube.at(Face(i)))
			{
				vert.x += worldX;
				vert.y += worldY;
				vert.z += worldZ;
				vert.r = aoQuad[j];
				vert.g = aoQuad[j];
				vert.b = aoQuad[j];
				inst.meshData.push_back(vert);
				++j;
			}
		}
	}
}

bool Chunk::isAir(BYTE block)
{
	return(block == 0);
}