#pragma once
#include "stdafx.h"
#include "chunk.h"
#include "safe_queue.h"
#include "map3d.h"
#include "lib/fastnoise/FastNoiseSIMD.h"
#include "world.h"

class World;
class WorldGen{
public:
	WorldGen(std::weak_ptr<World> world);
	~WorldGen();
	void beginGeneration();
	void seedGenQueue(const Point &seed);

	std::thread generationThread;
	std::unordered_map<Point, bool, hashFunc, equalsFunc> genCheckCache;
	
	SafeQueue<Point> genQueueIn;
private:
	FastNoiseSIMD* myNoise;
	std::weak_ptr<World> world;
	std::unordered_map<Point, float*, hashFunc, equalsFunc> heightMapCache;
	
	Chunk* generateChunk(const Point &chunkPos, std::shared_ptr<World> wp);
	Chunk* initializeChunk(const Point &chunkPos);
	void reseedQueue(const Point &center);
	float* genHeightMap(int x, int z);
	void setupAdjacents(Chunk *chunk);
};