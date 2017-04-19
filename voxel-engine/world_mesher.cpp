#include "stdafx.h"
#include "world_mesher.h"
#include "world.h"

WorldMesher::WorldMesher(WinInfo* winInfo, std::weak_ptr<World> world)
	: winInfo(winInfo), world(world), meshMutex()
{
	for (int i = 0; i < WORKER_COUNT; ++i)
		workerChunks[i] = nullptr;

	masterMesh = std::thread(&WorldMesher::meshDispatcher, this);
}

WorldMesher::~WorldMesher()
{
	pausing = true;
	for (int i = 0; i < WORKER_COUNT; ++i)
	{
		workerChunks[i] = nullptr;
	}
	for (auto &thread : workerThreads)
		if (thread.joinable())
			thread.join();
	if (masterMesh.joinable())
		masterMesh.join();
}

void WorldMesher::pauseWorkers(std::unique_lock<std::mutex> *lock)
{
	pausing = true;
	if (!lock->owns_lock())
		lock->lock();
	for (int i = 0; i < WORKER_COUNT; ++i)
	{
		workerChunks[i] = nullptr;
	}
}

void WorldMesher::resumeWorkers(std::unique_lock<std::mutex> *lock)
{
	if (lock->owns_lock())
		lock->unlock();
	pausing = false;
}

void WorldMesher::clearQueues()
{
	meshQueue.clear();
}

void WorldMesher::meshDispatcher()
{
	///Let the other threads set up before starting this
	for(int i = 0; i < WORKER_COUNT; ++i)
		workerThreads[i] = std::thread(&WorldMesher::meshingProcess, this, i);

	std::unique_lock<std::mutex> lock(meshMutex);
	while (!winInfo->shuttingDown)
	{
		
		if (!meshQueue.empty() && !pausing) 
		{
			for (int i = 0; i < WORKER_COUNT; ++i)
			{
				if (workerChunks[i] == nullptr)
				{
					if (!lock.owns_lock())
						lock.lock();
					if(!meshQueue.empty())
						workerChunks[i] = meshQueue.dequeue();
				}
			}
		}
		bool anyActive = false;
		for (int i = 0; i < WORKER_COUNT; ++i)
			if (workerChunks[i] != nullptr)
				anyActive = true;
		if (!anyActive)
		{
			if (lock.owns_lock())
				lock.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	for (int i = 0; i < WORKER_COUNT; ++i)
		workerChunks[i] = nullptr;
	for (auto &thread : workerThreads)
		if (thread.joinable())
			thread.join();
}

///Meshing process handled in its own thread
void WorldMesher::meshingProcess(int threadID)
{
	while (!winInfo->shuttingDown) 
	{
		if (workerChunks[threadID] != nullptr)
		{
			if (workerChunks[threadID]->generated && !workerChunks[threadID]->meshed)
				workerChunks[threadID]->mesh();
			workerChunks[threadID] = nullptr;
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}