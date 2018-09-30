#include "stdafx.h"
#include "util.h"
#include "world_gen.h"
#include "world.h"
#include "chunk.h"
#include "client/window.h"

WorldGen::WorldGen(std::weak_ptr<World> world) : world(world)
{
	myNoise = FastNoiseSIMD::NewFastNoiseSIMD();
	srand((unsigned int)time(NULL));
	myNoise->SetSeed(rand());
	myNoise->SetPerturbAmp(2.0f);
	myNoise->SetPerturbFrequency(0.5f);
	myNoise->SetPerturbFractalOctaves(8);
	myNoise->SetPerturbFractalLacunarity(2.0f);
	myNoise->SetPerturbFractalGain(0.5f);
	myNoise->SetCellularReturnType(myNoise->Distance2Add);
	myNoise->SetCellularDistanceFunction(myNoise->Natural);
	seedGenQueue(Point(0, 0, 0));

	generationThread = std::thread(&WorldGen::beginGeneration, this);
}

WorldGen::~WorldGen()
{
  std::cout << "end gen" << std::endl;
	if (generationThread.joinable())
		generationThread.join();

	for (auto &heightMap : heightMapCache)
		delete[] heightMap.second;
}

void WorldGen::beginGeneration()
{
	while (!glfwWindowShouldClose(Window::getGLFW()))
	{
		if (!genQueueIn.empty())
		{
			std::shared_ptr<World> wp = world.lock();
			std::lock_guard<std::mutex> lock(wp->genMutex);
			Point genItem = genQueueIn.dequeue();
			Chunk* newChunk = initializeChunk(genItem);
			if (!newChunk->meshed && newChunk->airBlocks < CHUNK_EXPANSE) {
				wp->addToMeshQueue(newChunk);
			}
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

///Each seed will queue a new generation in the adjacent chunk positions as long as their distance
///is less than the generation distance from the generation center
void WorldGen::seedGenQueue(const Point &seed)
{
	genQueueIn.enqueue(seed);
	genCheckCache[seed] = true;
}

void WorldGen::reseedQueue(const Point &center)
{
	std::shared_ptr<World> wp = world.lock();

	for(const Point &p : unitDirection)
	{
		Point adj = p + center;
		Chunk *c = wp->getChunkAt(adj);
		double dist = adj.distance(wp->generationCenter);
		if (dist <= WORLD_SIZE_SQ && !genCheckCache[adj])
			seedGenQueue(adj);
	}
}

void WorldGen::setupAdjacents(Chunk *chunk)
{
	std::shared_ptr<World> wp = world.lock();
	Chunk *plX = wp->getChunkAt(chunk->chunkPos + unitDirection[plusX]);
	Chunk *minX = wp->getChunkAt(chunk->chunkPos + unitDirection[minusX]);
	Chunk *plY = wp->getChunkAt(chunk->chunkPos + unitDirection[plusY]);
	Chunk *minY = wp->getChunkAt(chunk->chunkPos + unitDirection[minusY]);
	Chunk *plZ = wp->getChunkAt(chunk->chunkPos + unitDirection[plusZ]);
	Chunk *minZ = wp->getChunkAt(chunk->chunkPos + unitDirection[minusZ]);

	chunk->plusX = plX;
	chunk->minusX = minX;
	chunk->plusY = plY;
	chunk->minusY = minY;
	chunk->plusZ = plZ;
	chunk->minusZ = minZ;
}

Chunk* WorldGen::initializeChunk(const Point &chunkPos)
{
	std::shared_ptr<World> wp = world.lock();
	Chunk* chunk =  wp->getChunkAt(chunkPos);
	if (chunk->meshed)
	{
		reseedQueue(chunkPos);
		return(chunk);
	}
	
	std::vector<Chunk*> chunks;
	for (int x = -1; x <= 1; ++x)
		for (int y = -1; y <= 1; ++y)
			for (int z = -1; z <= 1; ++z)
			{
				Chunk *c = generateChunk(chunkPos + Point(x, y, z), wp);
				chunks.push_back(c);
			}
	for (auto &c : chunks)
		setupAdjacents(c);

	reseedQueue(chunkPos);
	return(chunk);
}

Chunk* WorldGen::generateChunk(const Point &chunkPos, std::shared_ptr<World> wp)
{
	//std::shared_ptr<World> wp = world.lock();
	Chunk *chunk = wp->getChunkAt(chunkPos);
	if (chunk->generated)
	{
		return (chunk);
	}
	
	Point heightPoint = Point(chunkPos.x, 0, chunkPos.z);
	float* heightMap = heightMapCache[heightPoint];
	if (heightMap == nullptr)
	{
		heightMapCache[heightPoint] = genHeightMap(heightPoint.x, heightPoint.z);
		heightMap = heightMapCache[heightPoint];
	}
	//float* noiseData = myNoise->GetSimplexFractalSet(chunkPos.x*CHUNK_SIZE, chunkPos.y*CHUNK_SIZE, chunkPos.z*CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);

	chunk->data = new BYTE[CHUNK_EXPANSE];
	int airBlocks = CHUNK_EXPANSE;
	for (int i = 0; i < CHUNK_EXPANSE; ++i)
	{
		int blockX = i / CHUNK_SIZE_SQ;
		int b = i - CHUNK_SIZE_SQ * blockX;
		int blockY = b / CHUNK_SIZE;
		int blockZ = b % CHUNK_SIZE;
		int height = (int)heightMap[blockX*CHUNK_SIZE + blockZ];
		int blockHeight = (blockY + chunkPos.y*CHUNK_SIZE) - SEA_LEVEL;
		/*if (blockHeight == 0)
		{
			chunk->data[i] = 2;
			--airBlocks;
		}
		else
			chunk->data[i] = 0;*/
		if (blockHeight > height)
		{
			chunk->data[i] = 0;
		}
		//else if (noiseData[i] <= AIR_THRESHOLD)
		//{
		//	chunk->data[i] = 0;
		//}
		else
		{
			if (blockHeight == height)
				chunk->data[i] = 1;
			else
				chunk->data[i] = 2;
			--airBlocks;
		}
	}
	//FastNoiseSIMD::FreeNoiseSet(noiseData);
	chunk->chunkPos = chunkPos;
	chunk->airBlocks = airBlocks;
	chunk->generated = true;
	return(chunk);
}

float* WorldGen::genHeightMap(int x, int z)
{
	float *noise/*, *noise2*/;
	myNoise->SetPerturbType(myNoise->GradientFractal);
	
	noise = myNoise->GetValueSet(x*CHUNK_SIZE, 0, z*CHUNK_SIZE, CHUNK_SIZE, 1, CHUNK_SIZE, 0.2f);
	myNoise->SetPerturbType(myNoise->None);
	//noise2 = myNoise->GetValueSet(x*CHUNK_SIZE, 0, z*CHUNK_SIZE, CHUNK_SIZE, 1, CHUNK_SIZE, 0.2f);
	const int HEIGHT_MAP_SIZE = CHUNK_SIZE*CHUNK_SIZE;
	float* heightMap = new float[HEIGHT_MAP_SIZE];
	for (int i = 0; i < HEIGHT_MAP_SIZE; ++i)
	{
		heightMap[i] = (noise[i] /** noise2[i]*/) * HEIGHT_SCALE;
	}

	FastNoiseSIMD::FreeNoiseSet(noise);
	//FastNoiseSIMD::FreeNoiseSet(noise2);
	return(heightMap);
}
