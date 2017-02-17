#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Game/Block.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/IntVector3.hpp"
#include <math.h>
#include "BlockInfo.hpp"

const int AMT_FACES_PER_BLOCK = 6;
const int AMT_VERTICES_PER_FACE = 4;
const int AMT_VERTICES_PER_BLOCK = AMT_FACES_PER_BLOCK*AMT_VERTICES_PER_FACE;


const int VERTICES_PER_CHUNK=BLOCKS_PER_CHUNK*AMT_VERTICES_PER_BLOCK;
const float BLOCK_CUBE_SIZE = 1.f;
const float CHUNK_DIMENSION_X=(float)BLOCK_CUBE_SIZE*(float)CHUNK_BLOCKS_X;
const float CHUNK_DIMENSION_Y=(float)BLOCK_CUBE_SIZE*(float)CHUNK_BLOCKS_Y;
const float	INV_CHUNK_DIMENSION_X=1.f/CHUNK_DIMENSION_X;
const float	INV_CHUNK_DIMENSION_Y=1.f/CHUNK_DIMENSION_Y;
const float CHUNK_AREA=BLOCK_CUBE_SIZE*BLOCK_CUBE_SIZE*BLOCKS_PER_CHUNK;

//TODO: For perlin noise, either load seed from file or generate once then save.

const int GRASS_LEVEL = CHUNK_BLOCKS_Z/2;
const int DIRT_LEVEL = CHUNK_BLOCKS_Z*7/16;

typedef IntVector2 ChunkCoords;

class ChunkInc: public ChunkCoords
	{
	public:
		ChunkInc(int newx = 0,int newy = 0) : ChunkCoords(newx,newy) {}
		void rotate90DegreesLeft();
		void operator ++();
	};

constexpr unsigned char CURRENT_CHUNK_VERSION = 0;

struct ChunkFileHeader
	{
	unsigned char m_version;
	unsigned char m_bitCountX;
	unsigned char m_bitCountY;
	unsigned char m_bitCountZ;
	unsigned char* asArray() { return &m_version; }
	};

class Chunk
	{
	public:
		Chunk();
		Chunk(const IntVector2& chunkcoords);
		~Chunk();

		void setWestNeighbor(Chunk* nbor);
		void setEastNeighbor(Chunk* nbor);
		void setSouthNeighbor(Chunk* nbor);
		void setNorthNeighbor(Chunk* nbor);

		void update(float deltaSeconds);

		void render() const;
			void drawDebugChunkBounds() const;

		void populateBlocks();
		void rebuildVerts();
			void rebuildVertsForSingleBlock(const IntVector3& pos);
			void bindVertsToVbo();
		void genBlockAt(const IntVector3& pos, int height=GRASS_LEVEL, BLOCK::Type surfaceType=BLOCK::GRASS);
		void addBlockStackAt(const IntVector3& pos,BLOCK::Type surfaceType=BLOCK::GRASS);
		void addBlockOfTypeAt(BLOCK::Type blocktype,const IntVector3& pos);

		AABB3 calcBlockBoundsInChunk(const IntVector3& blockPos);
		AABB3 calcBlockBoundsInWorld(const IntVector3& blockPos);
		AABB3 calcChunkBoundsInWorld();

		Block* getBlockAddrByIndex(int blockIndex);
		Block* getBlockAddrAt(const IntVector3& pos);
		static int getBlockIndexForLocalCoords(const IntVector3& pos);
		static IntVector3 getLocalCoordsForBlockIndex(int index);
		Vector3 calcMyPositionInWorld() const;
		Vector2 convertBlockCoordsToWorldCoords(int x,int y);
		IntVector2 convertBlockCoordsToWorldCoordsInt(const IntVector2& pos);
		Vector3 convertBlockCoordsToWorldCoords3(const IntVector3& pos);
		IntVector2 convertWorldCoordsToBlockCoords(const Vector2& pos);
		IntVector3 convertWorldCoordsToBlockCoords3(const Vector3& pos);

		Vector3 calcBlockCenter(const IntVector3& blockCoords);

		void populateFromSineWave();
			int calcSineHeight(int x,int y);
		void populateFromNoise(unsigned int seed);
		static int flattenNoise(int rawNoise);
		static int getNoiseHeightAt(int x,int y, int seed);
		static int getRawHeightAt(int x,int y, unsigned int seed);
		static float getBlockMoisture(int x,int y,unsigned int seed);
		static Vector2 getBlockHeightRange(int x,int y,unsigned int seed);
		static float getBaseTemperatureForBlock(int x,int y);
		static BLOCK::Type calcBiomeSurfaceType(int x,int y,int z,unsigned int seed);

		std::string buildMyFileName();

		void debugAddStoneStackAt(const Vector3& pos);
		bool populateFromFile();
		bool decodeFileBuffer(std::vector<unsigned char>& buffer,const std::vector<unsigned char>& encodedBuffer);
		bool populateFromRawData(const std::vector<unsigned char>& buffer);

		void saveToFile();
		void buildFileBufferUnencoded(std::vector<unsigned char>& buffer);
		void encodeFileBuffer(std::vector<unsigned char>& encodedBuffer, const std::vector<unsigned char>& buffer);

		/*bool isBlockHiddenAt(const IntVector3& pos);*/
		bool doesNeighborBlockObstructView(const BlockInfo& blockInfo);

		bool isDirty();

		bool wasChanged() { return m_wasChanged; }
		void setToWasChanged() { m_wasChanged=true; }
		void updateLightingAfterBlockPlacementAt(const IntVector3& blockCoords);
		void updateLightingAfterBlockRemovalAt(const IntVector3& blockCoords);

		static bool isOutOfBounds(const IntVector3& blockCoords,IntVector3& chunkOffset);

		//lighting methods
		void identifySkyBlocks();
			void identifySkyBlocksForColumn(int x,int y);
				void clearSkyBitsForColumn(int x,int y);
		void initializeLighting();
			void resetAllLightValuesToZero();
			void initializeLightAndDirtyForSkyBlocks();
			void initLightAndDirtyForSelfLightedBlocks();
			void enqueueAllLightDirtyBlocks();
				void enqueueBorderingLightDirtyBlocks();
					void enqueueEastLightDirtyBlocks();
					void enqueueWestLightDirtyBlocks();
					void enqueueSouthLightDirtyBlocks();
					void enqueueNorthLightDirtyBlocks();
			void makeNeighborsDirty();
		void enqueueLightDirtyBlockRange(const IntVector3& mins,const IntVector3& maxs);

		std::vector<Vector3> calcCornersInWorldCoords();
		std::vector<Vector3> calcCornersInCameraCoords(const Vector3& camPos);
		bool isAnyCornerAbovePlane(const Vector3& normal, const Vector3& camPos);
		bool isAnyCornerWithinRangeOfWorldPoint(const Vector3& camPos, float radius);
	public:
		//TODO: initialize these neighbor pointers
		Chunk* m_eastNeighbor;
		Chunk* m_westNeighbor;
		Chunk* m_southNeighbor;
		Chunk* m_northNeighbor;

		bool m_isVboDirty;
		unsigned int m_vboID;
		int m_amtVerticesInVbo;
		bool m_wasChanged;
		std::vector<Vertex3> m_verts;
		Block m_blocks[BLOCKS_PER_CHUNK];
		ChunkCoords m_chunkCoords;
private:
	};

inline Block* Chunk::getBlockAddrAt(const IntVector3& pos)
	{
	return &m_blocks[getBlockIndexForLocalCoords(pos)];
	}

inline Vector2 Chunk::convertBlockCoordsToWorldCoords(int x,int y)
	{
	float wx=(float)(x+m_chunkCoords.x*CHUNK_BLOCKS_X);
	float wy=(float)(y+m_chunkCoords.y*CHUNK_BLOCKS_Y);
	return Vector2(wx,wy);
	}

inline Vector3 Chunk::convertBlockCoordsToWorldCoords3(const IntVector3& pos)
	{
	float wx=(float)(pos.x+m_chunkCoords.x*CHUNK_BLOCKS_X);
	float wy=(float)(pos.y+m_chunkCoords.y*CHUNK_BLOCKS_Y);
	float wz=(float)pos.z;
	return Vector3(wx,wy,wz);
	}

inline IntVector2 Chunk::convertWorldCoordsToBlockCoords(const Vector2& pos)
	{
	int bx=(int)floor(pos.x)-m_chunkCoords.x*CHUNK_BLOCKS_X;
	int by=(int)floor(pos.y)-m_chunkCoords.y*CHUNK_BLOCKS_Y;
	return IntVector2(bx,by);
	}

inline IntVector3 Chunk::convertWorldCoordsToBlockCoords3(const Vector3& pos)
	{
	int bx=(int)floor(pos.x)-m_chunkCoords.x*CHUNK_BLOCKS_X;
	int by=(int)floor(pos.y)-m_chunkCoords.y*CHUNK_BLOCKS_Y;
	int bz=(int)floor(pos.z);
	return IntVector3(bx,by,bz);
	}

inline IntVector3 Chunk::getLocalCoordsForBlockIndex(int index)
	{
	IntVector3 result=IntVector3::ZERO;
	result.x=index&X_MASK_BITS;
	result.y=(index&Y_MASK_BITS)>>CHUNK_Y_MASK_OFFSET;
	result.z=(index&Z_MASK_BITS)>>CHUNK_Z_MASK_OFFSET;
	return result;
	}

inline int Chunk::getBlockIndexForLocalCoords(const IntVector3& pos)
	{
	int result=0;
	result |= (pos.z & Z_MASK_BITS_UNSHIFTED);
	result <<= CHUNK_Y_BITS;
	result |= (pos.y & Y_MASK_BITS_UNSHIFTED);
	result <<= CHUNK_X_BITS;
	result |= (pos.x & X_MASK_BITS_UNSHIFTED);
	return result;
	}

inline Vector3 Chunk::calcMyPositionInWorld() const
	{
	Vector3 result;
	result.x=(float)m_chunkCoords.x*(float)CHUNK_DIMENSION_X;
	result.y=(float)m_chunkCoords.y*(float)CHUNK_DIMENSION_Y;
	result.z=0.f;
	return result;
	}

inline int Chunk::getNoiseHeightAt(int x,int y,int seed)
	{
	int rawNoise=getRawHeightAt(x,y,seed);
	int result=flattenNoise(rawNoise);
	return result;
	}

inline void Chunk::addBlockOfTypeAt(BLOCK::Type blocktype,const IntVector3& pos)
	{
	Block& block=*getBlockAddrAt(pos);
	// 	block.m_pos=pos;
	block.setType((unsigned char)blocktype);
	}

inline void ChunkInc::rotate90DegreesLeft()
	{
	int tmp=x;
	x=-y;
	y=tmp;
	}

inline void ChunkInc::operator++()
	{
	if (x>0)
		x++;
	else if (x<0)
		x--;
	if (y>0)
		y++;
	else if (y<0)
		y--;
	}


inline int Chunk::calcSineHeight(int x,int y)
	{
	Vector2 worldpos=convertBlockCoordsToWorldCoords(x,y);
	float t = 0.1f + (1.f + sinf(0.1f * worldpos.x)) * 0.5f * 0.8f;
	return (int)((float)CHUNK_BLOCKS_Z*t);
	}

inline bool Chunk::isDirty()
	{
	return m_isVboDirty;
	}

