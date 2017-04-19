#pragma once
#include "stdafx.h"
#include "map3d.h"
#include "safe_queue.h"
#include "engine_win32.h"
#include "chunk.h"
#include "world_gen.h"
#include "util.h"
#include "world_mesher.h"

class WorldGen;
class Point;
class WorldMesher;
class World {
private:
	std::unordered_map<Point, Chunk, hashFunc, equalsFunc> world;
	WinInfo *winInfo;

	void checkReCenter(void);
	void tickWorld(void);
	void setNewCenter(Point center);
public:
	World(WinInfo *winInfo);
	~World();
	void addToMeshQueue(Chunk* chunk);
	Chunk* getChunkAt(Point &chunkPos);
	static Point toWorldPos(Point &chunkPos, Point &blockPos);
	static BYTE blockAt(Chunk *chunk, Point blockPos);
	static BYTE blockAt(Chunk *chunk, int x, int y, int z);

	Point generationCenter;
	std::thread worldThread;
	
	std::shared_ptr<WorldGen> worldGen;
	std::shared_ptr<WorldMesher> worldMesher;
	mutable std::mutex genMutex;
};
const Point unitDirection[] = { Point(1, 0, 0), Point(-1, 0, 0), Point(0, 1, 0), Point(0, -1, 0), Point(0, 0, 1), Point(0, 0, -1) };