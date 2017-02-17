#pragma once
#include "Game/Chunk.hpp"
#include "Game/BlockInfo.hpp"
#include "Game/Player.hpp"
#include <map>
#include <queue>

const float CHUNK_VISIBILITY_DISTANCE = 28.f;
const int MAX_CHUNKS_LOADED = (int)(0.785f*4.f*CHUNK_VISIBILITY_DISTANCE*CHUNK_VISIBILITY_DISTANCE + 0.5f);
const float IDEAL_CHUNK_PCT_OF_MAX = 0.85f;
const int IDEAL_CHUNKS_LOADED = (int)((float)MAX_CHUNKS_LOADED*IDEAL_CHUNK_PCT_OF_MAX);
const float CHUNK_UNITS_FROM_CAMERA_TO_ALWAYS_DRAW=2.f;
const float DISTANCE_FROM_CAMERA_TO_ALWAYS_DRAW = BLOCK_CUBE_SIZE*(float)(CHUNK_BLOCKS_X>CHUNK_BLOCKS_Y?CHUNK_BLOCKS_X:CHUNK_BLOCKS_Y)
                                                 *CHUNK_UNITS_FROM_CAMERA_TO_ALWAYS_DRAW;

const int LIGHT_STEP=1;
const int DARKEST_POSSIBLE_LIGHT=2;

struct RaycastResult
	{
	Chunk* m_impactChunk;
	int m_blockIndex;
	IntVector3 m_impactLocalCoords;
	float m_impactFraction;
	Vector3 m_impactPos;
	Vector3 m_impactFaceNormal;
	RaycastResult()
		:	m_impactChunk(nullptr)
		,	m_blockIndex(-1)
		,	m_impactLocalCoords(0,0,0)
		,	m_impactFraction(0.f)
		,	m_impactPos(0.f,0.f,0.f)
		,	m_impactFaceNormal(0.f,0.f,0.f)
		{

		}
	bool wasHit() { return m_impactChunk!=nullptr; }
	};

typedef IntVector2 CountyCoords;

enum CityNeighborType
	{
	NORTH_NEIGHBOR=0,
	SOUTH_NEIGHBOR=1,
	EAST_NEIGHBOR=2,
	WEST_NEIGHBOR=3,
	AMT_NEIGHBORS,
	};


struct City
	{

	City(const IntVector2& worldPos,int extentRadius,int verticality)
		: m_centerPos(worldPos),m_extentRadius(extentRadius),m_verticality(verticality)
		{
		}
	int estimatePopulation()
		{
		return m_extentRadius*m_extentRadius*m_verticality;
		}
	bool doesContainBlock(const IntVector2& blockPos)
		{
		int dx=blockPos.x-m_centerPos.x,dy=blockPos.y-m_centerPos.y;
		int dx2=dx*dx,dy2=dy*dy;
		int distFromCityCenterSquared=dx2+dy2;
		return distFromCityCenterSquared<=m_extentRadius*m_extentRadius;
		}

	CountyCoords m_countyPos;
	IntVector2 m_centerPos;
	int m_extentRadius;
	int m_verticality;
	City* nsewNeighbors[4];
	};


typedef std::map<CountyCoords,City*> CityGlossary;


class World
	{
	public:
		/*Chunk *m_chunk;*/
		World(const Vector3* campos);
		~World();
		Chunk* getChunkAtCoords(const ChunkCoords& chunkCoords);

		ChunkCoords calcChunkCoordsFromCameraPos();
		ChunkCoords calcChunkCoordsFromPlayerPos();

		ChunkCoords getChunkCoordsFromWorldPos(const Vector3& pos);
		bool areChunkCoordsInFiniteWorldBounds(const ChunkCoords& chunkCoords);
		void activateChunk(const ChunkCoords& chunkCoords);
		void deactivateChunk(Chunk* c);
			void removeChunkFromMap(Chunk* c);
		void	findAndActivateClosestMissingChunk(const ChunkCoords& centerChunkPos);
			ChunkCoords findClosestMissingChunkTo(const ChunkCoords& centerChunkPos);
				bool isChunkMissingAt(const ChunkCoords& potentialMissingChunkCoords);
				ChunkCoords calcClosestMissingChunkOutOfTo(const std::vector<ChunkCoords>& missingChunks,const ChunkCoords& centerChunkCoords);
		void findAndDeactivateFurthestChunk(const ChunkCoords& centerChunkPos);
			Chunk* findFurthestChunkFrom(const ChunkCoords& centerChunkPos);

		RaycastResult raycast(const Vector3& start,const Vector3& dir,float len, bool shouldTraceRay=true);
		bool isWorldPositionInSolid(const Vector3& pos);
		Chunk* getChunkForWorldPos(const Vector3& pos);
		BlockInfo getBlockInfoForWorldPos(const Vector3& pos);

		void render();
			std::vector<Chunk*> getListOfFrustrumCulledChunks();
		
		float calcNoise(const IntVector2& pos,float boxSize);
		float calcNoise(const IntVector2& pos,float boxSize,int seed);
		int getDefaultSurfaceHeightAt(const IntVector2& centerPos);

		//city- and county-oriented methods
		City* getCityForCountyAt(const CountyCoords& countyPos);
		CountyCoords calcCountyCoordsForBlockAt(const IntVector2& blockPos);
		void initCities();
			IntVector2 calcCityPosForCountyAt(const CountyCoords& countyPos);
			IntVector2 getAbsoluteCityPosition(const CountyCoords& countyPos,const IntVector2& cityPosWithinCounty);
		City* getCityAtBlockPosition(const IntVector2& blockPos);
		int getCityDevelopmentHeightAt(const IntVector2& blockPos);
		IntVector2 calcCitySquareCenterAt(const IntVector2& blockPos);
		BLOCK::Type getCitySquareBuildingMaterialTypeAt(const IntVector2& blockPos);
		bool isCityBlockSidewalkAt(const IntVector2& blockPos);
		IntVector2 convertWorldBlockPosToCitySquarePos(const IntVector2& blockPos);
		bool isCityBlockRoadAt(const IntVector2& blockPos);
		int getBuildingFoundationHeightAt(const IntVector2& blockAbsPos);
		void jumpToNextCity();
		void initCityNeighborPointers();
		void clearCityNeighborPointers();

		//directions
		static int getOppositeDirectionTo(int i);

		//update methods
		void update(float deltaSeconds);
		void updateActivatedChunks(float deltaSeconds);
			void updateVertexArrays();
			void updateLighting();
				void updateBlockLighting(const BlockInfo& dirtyBlockInfo);
			void updateEntities(float deltaSeconds);
				void detectAndCorrectCollisions(float deltaSeconds);

		int getActiveChunkCount();

		void processInput(float deltaSeconds);
			
		bool gotoNeighboringBlock(ChunkCoords& neighborChunkCoords,IntVector3& neighborBlockCoords,
								  const ChunkCoords& chunkCoords,const IntVector3& blockCoords,const IntVector3& blockDisp);
		void deactivateAllChunks();
		IntVector3 calcWorldPos(const ChunkCoords& chunkPos,const IntVector3& blockPos);

		BlockInfo getBlockInfoAtWorldPos(const Vector3& absWorldPos);

		Block* getBlockAt(const IntVector3& absBlockPos);
		Chunk* getChunkAtWorldBlockCoords(const IntVector3& absBlockPos);

		void enqueueDirtyLightBlock(const BlockInfo& dirtyBlockInfo);
		int getCurrentOutdoorLight();

		void updateNeighborPointersForChunkAt(const ChunkCoords& chunkCoords);
		void prepareNeighborPointersForChunkDeactivation(Chunk* c);
		void keepChunkNeighborsUpToDate();
		void printCulledChunkPercentage(std::vector<Chunk*>* chunksToRender);
	public:
		unsigned int m_seed;
		std::map<ChunkCoords,Chunk*> m_chunks;
		std::deque<BlockInfo> m_dirtyLightingBlocks;
		Player* m_thePlayer;
		const Vector3* m_cameraPos;	
		
		CityGlossary m_cities; 
		bool m_haveCitiesBeenInitialized;
		CityGlossary::iterator m_cityJumpIter;

	private:
		void addNumberOfCitiesToDebugPrinter();
		int countCities();
	};
