#pragma once
#include "Engine/Math/Snowman.hpp"
#include <vector>

class Chunk;
struct Block;
class Rgba;

struct BlockInfo
	{
	Chunk* m_chunk;
	int m_blockIndex;

	BlockInfo(): m_chunk(nullptr), m_blockIndex(0) {}
	BlockInfo(Chunk*chunk, int blockIndex): m_chunk(chunk), m_blockIndex(blockIndex) {}
	BlockInfo(const BlockInfo& copy): m_chunk(copy.m_chunk), m_blockIndex(copy.m_blockIndex) {}

	Block* getBlock() const;

	void moveEast();
	void moveWest();
	void moveNorth();
	void moveSouth();
	void moveUp();
	void moveDown();
	BlockInfo getEast();
	BlockInfo getWest();
	BlockInfo getNorth();
	BlockInfo getSouth();
	BlockInfo getUp();
	BlockInfo getDown();
	int getLight();
	void makeNeighborhoodLightingDirty();
	void clearLightingDirty();
	void enqueueHorizontalDirtyLightNeighbors();
	void enqueueWestDirtyLightNeighbor();
	void enqueueEastDirtyLightNeighbor();
	void enqueueSouthDirtyLightNeighbor();
	void enqueueNorthDirtyLightNeighbor();
	void enqueueUpDirtyLightNeigbhor();
	void enqueueDirtyLightSelf();
	Rgba getFaceColorFromLightValue();
	void detectAndCorrectNeighborhoodCollisions(float deltaSeconds,Snowman& snowmanBounds,Vector3& vel);
	void detectAndCorrectBlockCollision(float deltaSeconds,Snowman& snowmanBounds,Vector3& vel);
	std::vector<BlockInfo> getBlockHood();
	AABB3 calcBounds();
	};

