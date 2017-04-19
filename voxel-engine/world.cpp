#include "stdafx.h"
#include "world_gen.h"
#include "world.h"
#include "camera.h"

World::World(WinInfo *winInfo) : winInfo(winInfo), genMutex()
{
	worldThread = std::thread(&World::tickWorld, this);
}

World::~World()
{
	std::lock_guard<std::mutex> genLock(genMutex);
	std::unordered_map<Point, Chunk, hashFunc, equalsFunc>::iterator it = world.begin();
	while(it != world.end())
	{
		it->second.chunkMesh = nullptr;
		delete[] it->second.data;
		it = world.erase(it);
	}
}

void World::tickWorld()
{
	while (!winInfo->shuttingDown)
	{
		checkReCenter();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void World::checkReCenter()
{
	const int NEW_CENTER_DISTANCE = (int)pow(1, 2);
	Point pos = getPosition();
	int distFromCenter = (int)pos.distance(generationCenter);
	if (distFromCenter > NEW_CENTER_DISTANCE)
		setNewCenter(pos);
}

void World::setNewCenter(Point center)
{	
	///Lock these threads because we are deleting stuff
	std::lock_guard<std::mutex> genLock(genMutex);
	std::unique_lock<std::mutex> meshLock(worldMesher->meshMutex);
	worldMesher->pauseWorkers(&meshLock);
	worldGen->genQueueIn.clear();
	worldMesher->clearQueues();
	generationCenter = center;
	std::unordered_map<Point, Chunk, hashFunc, equalsFunc>::iterator it = world.begin();
	while(it != world.end())
	{
		Chunk *chunk = &it->second;
		if (chunk->generated)
		{
			double chunkDist = generationCenter.distance(chunk->chunkPos);
			///If it is outside the render distance then we either delete mesh or both mesh and data
			if (chunkDist > WORLD_SIZE_SQ)
			{	
				if (chunk->meshed)
				{
					removeChunkMeshQueue(chunk->chunkMesh);
					chunk->chunkMesh = nullptr;
					chunk->meshed = false;
				}
				chunk->generated = false;
				delete[] chunk->data;
				chunk->data = nullptr;	
			}
		}
		if (!chunk->generated || chunk->data == nullptr)
			it = world.erase(it);
		else
			++it;
	}
	worldGen->genCheckCache.clear();
	worldGen->seedGenQueue(generationCenter);
	worldMesher->resumeWorkers(&meshLock);
}

void World::addToMeshQueue(Chunk* chunk)
{
	worldMesher->meshQueue.enqueue(chunk);
}

Point World::toWorldPos(Point &chunkPos, Point &blockPos)
{
	int	x = chunkPos.x*CHUNK_SIZE + blockPos.x;
	int y = chunkPos.y*CHUNK_SIZE + blockPos.y;
	int z = chunkPos.z*CHUNK_SIZE + blockPos.z;
	Point worldPos = Point(x, y, z);
	return (worldPos);
}

BYTE World::blockAt(Chunk *chunk, Point blockPos)
{
	BYTE block = 0;
	if (blockPos.x < 0) { chunk = chunk->minusX; blockPos.x += CHUNK_SIZE; }
	if (blockPos.x >= CHUNK_SIZE) { chunk = chunk->plusX; blockPos.x -= CHUNK_SIZE; }
	if (blockPos.y < 0) { chunk = chunk->minusY; blockPos.y += CHUNK_SIZE; }
	if (blockPos.y >= CHUNK_SIZE) { chunk = chunk->plusY; blockPos.y -= CHUNK_SIZE; }
	if (blockPos.z < 0) { chunk = chunk->minusZ; blockPos.z += CHUNK_SIZE; }
	if (blockPos.z >= CHUNK_SIZE) { chunk = chunk->plusZ; blockPos.z -= CHUNK_SIZE; }
	if (chunk->data == nullptr) return(block);

	int blockInd = blockPos.oneDIndex(CHUNK_SIZE);
	if (blockInd >= 0 && blockInd < CHUNK_EXPANSE) 
		block = chunk->data[blockInd];

	return(block);
}

BYTE World::blockAt(Chunk *chunk, int x, int y, int z)
{
	BYTE block = 0;
	if (x < 0) { chunk = chunk->minusX; x += CHUNK_SIZE; }
	else if (x >= CHUNK_SIZE) { chunk = chunk->plusX; x -= CHUNK_SIZE; }
	if (y < 0) { chunk = chunk->minusY; y += CHUNK_SIZE; }
	else if (y >= CHUNK_SIZE) { chunk = chunk->plusY; y -= CHUNK_SIZE; }
	if (z < 0) { chunk = chunk->minusZ; z += CHUNK_SIZE; }
	else if (z >= CHUNK_SIZE) { chunk = chunk->plusZ; z -= CHUNK_SIZE; }
	if (chunk->data == nullptr)
		return(block);

	int blockInd = x * CHUNK_SIZE_SQ + y * CHUNK_SIZE + z;
	if (blockInd >= 0 && blockInd < CHUNK_EXPANSE) 
		block = chunk->data[blockInd];
	if (block < 0 || block > 2)
		int breakhere = 3;
	return(block);
}

Chunk* World::getChunkAt(Point &chunkPos)
{
	return(&world[chunkPos]);
}