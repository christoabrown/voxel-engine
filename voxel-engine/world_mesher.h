#pragma once
#include "stdafx.h"
#include "world.h"
#include "safe_queue.h"

class World;
const int WORKER_COUNT = 12;
class WorldMesher
{
private:
	void meshingProcess(int threadID);
	void meshDispatcher();

	std::thread masterMesh;
	std::weak_ptr<World> world;
	Chunk* workerChunks[WORKER_COUNT];
	std::thread workerThreads[WORKER_COUNT];
	
public:
	WorldMesher(std::weak_ptr<World> world);
	~WorldMesher();
	void pauseWorkers();
	void resumeWorkers();
	void clearQueues();

	SafeQueue<Chunk*> meshQueue;
	mutable std::mutex meshMutex;
	bool pausing = false;
};
