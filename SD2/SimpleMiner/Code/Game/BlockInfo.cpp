#include "Game/BlockInfo.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Chunk.hpp"
#include "Game/World.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include <sstream>

Block* BlockInfo::getBlock() const
	{
	return m_chunk? m_chunk->getBlockAddrByIndex(m_blockIndex): nullptr;
	}

void BlockInfo::moveEast()
	{
	int x=m_blockIndex&X_MASK_BITS;
	x = x >> CHUNK_X_MASK_OFFSET;
	if (x>=CHUNK_BLOCKS_X-1)
		{
		if (m_chunk != nullptr)
			m_chunk = m_chunk->m_eastNeighbor;
		}
	x++;
	x&=X_MASK_BITS_UNSHIFTED;
	x = x << CHUNK_X_MASK_OFFSET;
	m_blockIndex&=~X_MASK_BITS;
	m_blockIndex|=x;
	}

void BlockInfo::moveWest()
	{
	int x=m_blockIndex&X_MASK_BITS;
	x = x >> CHUNK_X_MASK_OFFSET;
	if (x<=0)
		{
		if (m_chunk != nullptr)
			m_chunk = m_chunk->m_westNeighbor;
		}
	x--;
	x&=X_MASK_BITS_UNSHIFTED;
	x = x << CHUNK_X_MASK_OFFSET;
	m_blockIndex&=~X_MASK_BITS;
	m_blockIndex|=x;
	}

void BlockInfo::moveNorth()
	{
	int y=m_blockIndex&Y_MASK_BITS;
	y = y >> CHUNK_Y_MASK_OFFSET;
	if (y>=CHUNK_BLOCKS_Y-1)
		{
		if (m_chunk != nullptr)
			m_chunk = m_chunk->m_northNeighbor;
		}
	y++;
	y&=Y_MASK_BITS_UNSHIFTED;
	y = y << CHUNK_Y_MASK_OFFSET;
	m_blockIndex&=~Y_MASK_BITS;
	m_blockIndex|=y;
	}

void BlockInfo::moveSouth()
	{
	int y=m_blockIndex&Y_MASK_BITS;
	y = y >> CHUNK_Y_MASK_OFFSET;
	if (y<=0)
		{
		if (m_chunk != nullptr)
			m_chunk = m_chunk->m_southNeighbor;
		}
	y--;
	y&=Y_MASK_BITS_UNSHIFTED;
	y = y << CHUNK_Y_MASK_OFFSET;
	m_blockIndex&=~Y_MASK_BITS;
	m_blockIndex|=y;
	}

void BlockInfo::moveDown()
	{
	int z=m_blockIndex&Z_MASK_BITS;
	z = z >> CHUNK_Z_MASK_OFFSET;
	if (z<=0)
		{
		m_chunk=nullptr;
		}
	z--;
	z&=Z_MASK_BITS_UNSHIFTED;
	z = z << CHUNK_Z_MASK_OFFSET;
	m_blockIndex&=~Z_MASK_BITS;
	m_blockIndex|=z;
	}

void BlockInfo::moveUp()
	{
	int z=m_blockIndex&Z_MASK_BITS;
	z = z >> CHUNK_Z_MASK_OFFSET;
	if (z>=CHUNK_BLOCKS_Z-1)
		{
		m_chunk=nullptr;
		}
	z++;
	z&=Z_MASK_BITS_UNSHIFTED;
	z = z << CHUNK_Z_MASK_OFFSET;
	m_blockIndex&=~Z_MASK_BITS;
	m_blockIndex|=z;
	}

BlockInfo BlockInfo::getEast()
	{
	BlockInfo result(*this);
	result.moveEast();
	return result;
	}

BlockInfo BlockInfo::getWest()
	{
	BlockInfo result(*this);
	result.moveWest();
	return result;
	}

BlockInfo BlockInfo::getNorth()
	{
	BlockInfo result(*this);
	result.moveNorth();
	return result;
	}

BlockInfo BlockInfo::getSouth()
	{
	BlockInfo result(*this);
	result.moveSouth();
	return result;
	}

BlockInfo BlockInfo::getUp()
	{
	BlockInfo result(*this);
	result.moveUp();
	return result;
	}

BlockInfo BlockInfo::getDown()
	{
	BlockInfo result(*this);
	result.moveDown();
	return result;
	}

int BlockInfo::getLight()
	{
	Block* block=getBlock();
	if (block)
		return block->getLight();
	else
		return 0;
	}

void BlockInfo::makeNeighborhoodLightingDirty()
	{
	BlockInfo upInfo  = getUp();
	BlockInfo downInfo= getDown();
	BlockInfo eastInfo= getEast();
	BlockInfo westInfo= getWest();
	BlockInfo southInfo=getSouth();
	BlockInfo northInfo=getNorth();

	Block*     upBlock =     upInfo.getBlock();
	Block*   downBlock =   downInfo.getBlock();
	Block*   eastBlock =   eastInfo.getBlock();
	Block*   westBlock =   westInfo.getBlock();
	Block*  southBlock =  southInfo.getBlock();
	Block*  northBlock =  northInfo.getBlock();

	if (upBlock&&!upBlock->isSolid()       && !upBlock->isSky())     upBlock->setLightDirtyBit(true);
	if (downBlock&&!downBlock->isSolid()   && !downBlock->isSky())   downBlock->setLightDirtyBit(true);
	if (eastBlock&&!eastBlock->isSolid()   && !eastBlock->isSky())   eastBlock->setLightDirtyBit(true);
	if (westBlock&&!westBlock->isSolid()   && !westBlock->isSky())   westBlock->setLightDirtyBit(true);
	if (southBlock&&!southBlock->isSolid() && !southBlock->isSky())  southBlock->setLightDirtyBit(true);
	if (northBlock&&!northBlock->isSolid() && !northBlock->isSky())  northBlock->setLightDirtyBit(true);
	}

void BlockInfo::clearLightingDirty()
	{
	Block*  block=getBlock();
	if(block)
		block->setLightDirtyBit(false);
	}

void BlockInfo::enqueueHorizontalDirtyLightNeighbors()
	{
	enqueueWestDirtyLightNeighbor();
	enqueueEastDirtyLightNeighbor();
	enqueueSouthDirtyLightNeighbor();
	enqueueNorthDirtyLightNeighbor();
	}

void BlockInfo::enqueueWestDirtyLightNeighbor()
	{
	BlockInfo bi=getWest();
	bi.enqueueDirtyLightSelf();
	}

void BlockInfo::enqueueEastDirtyLightNeighbor()
	{
	BlockInfo bi=getEast();
	bi.enqueueDirtyLightSelf();
	}

void BlockInfo::enqueueSouthDirtyLightNeighbor()
	{
	BlockInfo bi=getSouth();
	bi.enqueueDirtyLightSelf();
	}

void BlockInfo::enqueueNorthDirtyLightNeighbor()
	{
	BlockInfo bi=getNorth();
	bi.enqueueDirtyLightSelf();
	}

void BlockInfo::enqueueUpDirtyLightNeigbhor()
	{
	BlockInfo bi=getUp();
	bi.enqueueDirtyLightSelf();
	}

void BlockInfo::enqueueDirtyLightSelf()
	{
	Block* b=getBlock();
	if (b && !b->isOpaque())
		{
		b->setLight(0);
		g_theWorld->enqueueDirtyLightBlock(*this);
		}
	}

Rgba BlockInfo::getFaceColorFromLightValue()
	{
	Block* block=getBlock();
	if(block==nullptr) 
		return Rgba::BLACK;

	unsigned char gray=(unsigned char)block->getLight()*16;
	Rgba result=Rgba::getGray(gray,255);
// 	if(result==Rgba::BLACK)
// 		result=Rgba::MAGENTA;

	return result;
	}

void BlockInfo::detectAndCorrectNeighborhoodCollisions(float deltaSeconds,Snowman& snowmanBounds,Vector3& vel)
	{
	if (m_chunk==nullptr)
		return;
	std::vector<BlockInfo> blockHood=getBlockHood();

	//  6  7  8     15 16 17     24 25 26
	//  3  4  5     12 13 14     21 22 23
	//  0  1  2      9 10 11     18 19 20 
	int selfNeighborIndex=13;
	int faceNeighborIndices[6]={4,10,12,14,16,22};
	int edgeNeighborIndices[12]={1,3,5,7,9,11,15,17,19,21,23,25};
	int cornerNeighborIndices[8]={0,2,6,8,18,20,24,26};

	blockHood[selfNeighborIndex].detectAndCorrectBlockCollision(deltaSeconds,snowmanBounds,vel);
	for (int i=0;i<6;i++)
		blockHood[faceNeighborIndices[i]].detectAndCorrectBlockCollision(deltaSeconds,snowmanBounds,vel);
	for (int i=0;i<12;i++)
		blockHood[edgeNeighborIndices[i]].detectAndCorrectBlockCollision(deltaSeconds,snowmanBounds,vel);
	for (int i=0;i<8;i++)
		blockHood[cornerNeighborIndices[i]].detectAndCorrectBlockCollision(deltaSeconds,snowmanBounds,vel);
	}

void BlockInfo::detectAndCorrectBlockCollision(float /*deltaSeconds*/,Snowman& snowmanBounds,Vector3& vel)
	{
	if(m_chunk==nullptr)
		return;
	Block* block=getBlock();
	if(block==nullptr)
		return;
	if(!block->isSolid())
		return;
	AABB3 blockBounds=calcBounds();
	if(snowmanBounds.doesOverlapWith(blockBounds))
		{
		snowmanBounds.separateFrom(blockBounds,vel);
// 		std::stringstream ss;
// 		ss.str("");
// 		ss<<"Overlap Region: 0x"<<std::hex<<snowmanBounds.m_overlapRegionCode;
// 		g_theDebugMessagePrinter->addMessage(ss.str(),Rgba::MAGENTA);
/*		vel=Vector3::ZERO;*/
		}
	}

std::vector<BlockInfo> BlockInfo::getBlockHood()
	{
	std::vector<BlockInfo> result;

	for(int i=0;i<27;i++)
		result.push_back(*this);

	for (int z=0;z<3;z++)
		{
		for (int y=0;y<3;y++)
			{
			for (int x=0;x<3;x++)
				{
				int i=z*9+y*3+x;
				if      (x==0) result[i].moveWest();
				else if (x==2) result[i].moveEast();
				if      (y==0) result[i].moveSouth();
				else if (y==2) result[i].moveNorth();
				if      (z==0) result[i].moveDown();
				else if (z==2) result[i].moveUp();
				}
			}
		}

	return result;
	}

AABB3 BlockInfo::calcBounds()
	{
	AABB3 result;
	if (m_chunk==nullptr)
		return result;
	result.m_mins=Vector3(g_theWorld->calcWorldPos(m_chunk->m_chunkCoords,m_chunk->getLocalCoordsForBlockIndex(m_blockIndex)));
	Vector3 blockSizeVector(BLOCK_CUBE_SIZE,BLOCK_CUBE_SIZE,BLOCK_CUBE_SIZE);
	result.m_maxs=result.m_mins+blockSizeVector;
	return result;
	}
