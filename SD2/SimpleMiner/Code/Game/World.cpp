#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Noise.hpp"
#include <cassert>
#include <ctime>
#include <sstream>

int g_waterLevel=MAX_GENERATED_HEIGHT*37/100;
// int g_waterLevel=0;

bool g_doHorizonEffect=true;

World::World(const Vector3* campos)
	: m_cameraPos(campos)
	, m_dirtyLightingBlocks()
	, m_thePlayer(nullptr)
	, m_seed(0)
	, m_haveCitiesBeenInitialized(false)
	, m_cityJumpIter()
	{
/*	m_chunk=new Chunk();*/
	if(campos)
		m_thePlayer=new Player(*campos);

	}

World::~World()
	{
// 	delete m_chunk;
// 	m_chunk=nullptr;
	delete m_thePlayer;
	m_thePlayer=nullptr;
	}

bool operator < (const IntVector2& a, const IntVector2& b)
	{
	if(a.x<b.x)
		return true;
	if(b.x<a.x)
		return false;

	if(a.y<b.y)
		return true;
	
	return false;
	}

Chunk* World::getChunkAtCoords(const ChunkCoords& chunkCoords)
	{
	std::map<IntVector2,Chunk*>::iterator found;
	found=m_chunks.find(chunkCoords);
	if(found!=m_chunks.end())
		return found->second;
	else
		return nullptr;
	}

void World::activateChunk(const ChunkCoords& chunkCoords)
	{
	Chunk* newChunk=new Chunk(chunkCoords);
	m_chunks[chunkCoords]=newChunk;
	if(!areChunkCoordsInFiniteWorldBounds(chunkCoords))
		return;
	bool didLoad=newChunk->populateFromFile();
	if (!didLoad)
		{
		#if 0
		newChunk->populateFromSineWave();
		#else
		newChunk->populateFromNoise(m_seed);
		#endif
		}
	else
		{
		while(true)
			break;
		}

	updateNeighborPointersForChunkAt(chunkCoords);

	newChunk->identifySkyBlocks();
	newChunk->initializeLighting();
	}

void World::deactivateChunk(Chunk* c)
	{
	if(c==nullptr)
		return;
	prepareNeighborPointersForChunkDeactivation(c);
	removeChunkFromMap(c);
	if (c->wasChanged())
		c->saveToFile();
	delete c;
	}

void World::removeChunkFromMap(Chunk* c)
	{
	for (std::map<IntVector2,Chunk*>::iterator iter=m_chunks.begin();iter!=m_chunks.end(); iter++)
		{
		if (iter->second==c)
			{
			m_chunks.erase(iter);
			break;
			}
		}
	}

ChunkCoords World::calcChunkCoordsFromCameraPos()
	{
	Vector3 campos(*m_cameraPos);
	return getChunkCoordsFromWorldPos(campos);
	}

ChunkCoords World::calcChunkCoordsFromPlayerPos()
	{
	Vector3 playerPos(m_thePlayer->getPosition());
	return getChunkCoordsFromWorldPos(playerPos);
	}

ChunkCoords World::getChunkCoordsFromWorldPos(const Vector3& pos)
	{
	ChunkCoords result(0,0);

	result.x=(int)floor(pos.x*INV_CHUNK_DIMENSION_X);
	result.y=(int)floor(pos.y*INV_CHUNK_DIMENSION_Y);
	return result;
	}

bool World::areChunkCoordsInFiniteWorldBounds(const ChunkCoords& chunkCoords)
	{
	bool result =    chunkCoords.x>=0 && chunkCoords.y>=0
		          && chunkCoords.x<FINITE_WORLD_AMT_CHUNKS_X
		          && chunkCoords.y<FINITE_WORLD_AMT_CHUNKS_Y;
	return result;
	}

void World::findAndActivateClosestMissingChunk(const ChunkCoords& centerChunkPos)
	{
	ChunkCoords closestMissingChunkPos=findClosestMissingChunkTo(centerChunkPos);
	int sqrdist=closestMissingChunkPos.calcDistanceSquaredTo(centerChunkPos);
	if(sqrdist>CHUNK_VISIBILITY_DISTANCE*CHUNK_VISIBILITY_DISTANCE)
		return;
	activateChunk(closestMissingChunkPos);
	}

void World::findAndDeactivateFurthestChunk(const ChunkCoords& centerChunkPos)
	{
	Chunk* c = findFurthestChunkFrom(centerChunkPos);
	deactivateChunk(c);
	}

Chunk* World::findFurthestChunkFrom(const ChunkCoords& centerChunkPos)
	{
	if (m_chunks.size()==0)
		return nullptr;
	std::map<ChunkCoords,Chunk*>::iterator iter=m_chunks.begin();
	Chunk* result=iter->second;
	ChunkCoords furthestPos=iter->first;
	int furthestDistSqr=furthestPos.calcDistanceSquaredTo(centerChunkPos);
	iter++;
	for (;iter!=m_chunks.end();iter++)
		{
		ChunkCoords currentPos=iter->first;
		int currentDistSqr=currentPos.calcDistanceSquaredTo(centerChunkPos);
		if (currentDistSqr>furthestDistSqr)
			{
			furthestDistSqr=currentDistSqr;
			furthestPos=currentPos;
			result=iter->second;
			}		
		}
	return result;
	}

void World::render()
	{
	std::vector<Chunk*> chunksToRender=getListOfFrustrumCulledChunks();
	for (Chunk*chunk: chunksToRender)
		{
		chunk->render();
		}
	printCulledChunkPercentage(&chunksToRender);
	m_thePlayer->render();
	}

std::vector<Chunk*> World::getListOfFrustrumCulledChunks()
	{
	std::vector<Chunk*> result;
	std::vector<Vector3> viewFrustumPlaneNormals=g_theGame->getCamera().getViewFrustumPlaneNormals();

	for (std::map<IntVector2,Chunk*>::iterator iter=m_chunks.begin();iter!=m_chunks.end(); iter++)
		{
		Chunk* chunk=iter->second;
		bool isCloseToCamera=chunk->isAnyCornerWithinRangeOfWorldPoint(*m_cameraPos, DISTANCE_FROM_CAMERA_TO_ALWAYS_DRAW);
		bool isInViewFrustrum=true;
		for (Vector3 normal: viewFrustumPlaneNormals)
			{
			isInViewFrustrum=isInViewFrustrum&&chunk->isAnyCornerAbovePlane(normal, *m_cameraPos);
			}
		bool shouldPush=isInViewFrustrum||isCloseToCamera;
		if(shouldPush)
			result.push_back(chunk);
		}

	return result;
	}

bool World::isCityBlockRoadAt(const IntVector2& blockPos)
	{
	IntVector2 blockPosInSquare=blockPos;
	blockPosInSquare.x&=CITY_SQUARE_DIMENSION_MASK;
	blockPosInSquare.y&=CITY_SQUARE_DIMENSION_MASK;
	int left=CITY_SQUARE_HALF_ROAD_WIDTH;
	int bottom=left;
	int right=CITY_SQUARE_BLOCKS_XY-1-CITY_SQUARE_HALF_ROAD_WIDTH;
	int top=right;

	bool isRoad     = blockPosInSquare.x<left || blockPosInSquare.y<bottom
		           || blockPosInSquare.x>right || blockPosInSquare.y>top;
	bool result = isRoad;
	return result;
	}

int World::getBuildingFoundationHeightAt(const IntVector2& blockAbsPos)
	{
	IntVector2 centerPos=calcCitySquareCenterAt(blockAbsPos);
	return getDefaultSurfaceHeightAt(centerPos);
	}

void World::jumpToNextCity()
	{
	if(!m_haveCitiesBeenInitialized)
		return;
	m_cityJumpIter++;
	if (m_cityJumpIter==m_cities.end())
		m_cityJumpIter=m_cities.begin();

	City* jumpCity=m_cityJumpIter->second;
	IntVector3 cityPosInt(jumpCity->m_centerPos.x,jumpCity->m_centerPos.y,CHUNK_BLOCKS_Z+2);
	Vector3 cityPos(cityPosInt);

	m_thePlayer->teleportTo(cityPos);
	g_theGame->getCamera().m_pos=cityPos;
	}

void World::initCityNeighborPointers()
	{
	clearCityNeighborPointers();
		
	for (auto& entry : m_cities)
		{
		CountyCoords myCountyCoords=entry.first;
		City* city=entry.second;
		if (city==nullptr)
			continue;

		CountyCoords ccXInc(1,0);
		CountyCoords ccYInc(0,1);

		CountyCoords nsewCoords[4];
		nsewCoords[NORTH_NEIGHBOR]=myCountyCoords+ccYInc;
		nsewCoords[SOUTH_NEIGHBOR]=myCountyCoords-ccYInc;
		nsewCoords[EAST_NEIGHBOR]=myCountyCoords+ccYInc;
		nsewCoords[WEST_NEIGHBOR]=myCountyCoords-ccYInc;

		for (int i=0;i<AMT_NEIGHBORS;i++)
			{
			CityGlossary::iterator it=m_cities.find(nsewCoords[i]);
			if(it==m_cities.end())
				{
				city->nsewNeighbors[i]=nullptr;
				continue;
				}
			City* neighbor=m_cities.at(nsewCoords[i]);
			int j=World::getOppositeDirectionTo(i);
			city->nsewNeighbors[i]=neighbor;
			neighbor->nsewNeighbors[j]=city;
			}
		}
	}

void World::clearCityNeighborPointers()
	{
	for (auto& entry: m_cities)
		{
		City* city=entry.second;
		if(city==nullptr)
			continue;
		for (int i=0;i<AMT_NEIGHBORS;i++)
			city->nsewNeighbors[i]=nullptr;
		}
	}

int World::getOppositeDirectionTo(int i)
	{
	int result=i;
	if(i<0 || i>AMT_NEIGHBORS)
		return result;
	switch (i)
		{
		case NORTH_NEIGHBOR:
			result=SOUTH_NEIGHBOR;
			break;
		case SOUTH_NEIGHBOR:
			result=NORTH_NEIGHBOR;
			break;
		case EAST_NEIGHBOR:
			result=WEST_NEIGHBOR;
			break;
		case WEST_NEIGHBOR:
			result=EAST_NEIGHBOR;
			break;
		}
	return result;
	}

void World::update(float deltaSeconds)
	{
	if(!m_haveCitiesBeenInitialized)
		initCities();
	if(g_theDebugMessagePrinter)
		addNumberOfCitiesToDebugPrinter();
	processInput(deltaSeconds);
 	updateActivatedChunks(deltaSeconds);
	updateLighting();
	updateVertexArrays();
	updateEntities(deltaSeconds);
	}

float World::calcNoise(const IntVector2& pos,float boxSize)
	{
	int seed=(m_seed^0xf0f0f0f0);
	return calcNoise(pos,boxSize,seed);
	}

float World::calcNoise(const IntVector2& pos,float boxSize,int seed)
	{
	//calc noise
	float fx=pos.x;
	float fy=pos.y;
	float scale=boxSize;
	int numOctaves=10;
	float octavePersistence=0.3f;
	float octaveScale=2.5;
	bool shouldRenormalize=true;
	float fnoise = Compute2dPerlinNoise(fx,fy,scale,numOctaves,octavePersistence,octaveScale,shouldRenormalize,seed);
	fnoise+=1.f;
	fnoise*=0.5f;
	float result=fnoise;
	return result;
	}

int World::getDefaultSurfaceHeightAt(const IntVector2& centerPos)
	{
	return Chunk::getNoiseHeightAt(centerPos.x,centerPos.y,m_seed);
	}

City* World::getCityForCountyAt(const CountyCoords& countyPos)
	{
	City* result=m_cities.at(countyPos);
	return result;
	}

CountyCoords World::calcCountyCoordsForBlockAt(const IntVector2& blockPos)
	{
	CountyCoords result;
	result.x=blockPos.x/COUNTY_BLOCKS_XY;
	result.y=blockPos.y/COUNTY_BLOCKS_XY;
	return result;
	}

IntVector2 World::calcCityPosForCountyAt(const CountyCoords& countyPos)
	{
	float boxSize=10.f;
	float fnoise=calcNoise((IntVector2)countyPos,boxSize);

	//convert to position
	float countyBlockArea=(float)(COUNTY_BLOCKS_XY*COUNTY_BLOCKS_XY);
	fnoise*=(float)calcNoise(countyPos,boxSize);
	int noise=(int)(fnoise*(float)(COUNTY_BLOCKS_XY*COUNTY_BLOCKS_XY));
	int x=noise/COUNTY_BLOCKS_XY;
	int y=noise-(x*COUNTY_BLOCKS_XY);
	x=clampInt(x,0,COUNTY_BLOCKS_XY);
	y=clampInt(y,0,COUNTY_BLOCKS_XY);
	IntVector2 result=IntVector2(x,y);
	return result;
	}

IntVector2 World::getAbsoluteCityPosition(const CountyCoords& countyPos,const IntVector2& cityPosWithinCounty)
	{
	IntVector2 result;
	result=(IntVector2)countyPos*COUNTY_BLOCKS_XY;
	result+=cityPosWithinCounty;
	return result;
	}

City* World::getCityAtBlockPosition(const IntVector2& blockPos)
	{
	CountyCoords myCountyPos=calcCountyCoordsForBlockAt(blockPos);
	City* city=getCityForCountyAt(myCountyPos);
	if(city==nullptr || !city->doesContainBlock(blockPos))
		return nullptr;
	return city;
	}
// 
// int World::getCityDevelopmentHeightAt(const IntVector2& blockPos)
// 	{
// 	City* city=getCityAtBlockPosition(blockPos);
// 	if(city==nullptr)
// 		return 0;
// 	int amtFloors=city->m_verticality;
// 	int distFromCityCenter=absval(blockPos.x-city->m_centerPos.x)+absval(blockPos.y-city->m_centerPos.y);
// 	int invDist=clampInt(city->m_extentRadius-distFromCityCenter,0,city->m_extentRadius);
// 	amtFloors=clampInt(city->m_verticality*invDist/city->m_extentRadius,1,city->m_verticality);
// 	int blocksPerFloor=3;
// 	int result=amtFloors*blocksPerFloor;
// 	return result;
// 	}

int World::getCityDevelopmentHeightAt(const IntVector2& blockPos)
	{
	City* city=getCityAtBlockPosition(blockPos);
	if(city==nullptr)
		return 0;

	float maxFloorsFloat=(float)city->m_verticality;

	float dx=(float)(blockPos.x-city->m_centerPos.x);
	float dy=(float)(blockPos.y-city->m_centerPos.y);
	float dx2=dx*dx;
	float dy2=dy*dy;
	float r=(float)(city->m_extentRadius)*1.f;
	float r2=r*r;
	float invHeightScale=(dx2+dy2)/r2;
	float fnoise=calcNoise(blockPos,25)*0.2f;
	invHeightScale+=fnoise;
	invHeightScale=clampFloat(invHeightScale,0.f,1.f);
	float heightScale=1.f-invHeightScale;
	heightScale=smoothStartN(heightScale,4);

	int amtFloors=clampInt((int)(heightScale*(float)city->m_verticality),2,city->m_verticality);
	int blocksPerFloor=2;
	int result=amtFloors*blocksPerFloor;
	return result;
	}

IntVector2 World::calcCitySquareCenterAt(const IntVector2& blockPos)
	{
	IntVector2 result=blockPos;
	result.x-=(result.x&(CITY_SQUARE_DIMENSION_MASK));
	result.y-=(result.y&(CITY_SQUARE_DIMENSION_MASK));
	result.x+=CITY_SQUARE_BLOCK_CENTER_XY;
	result.y+=CITY_SQUARE_BLOCK_CENTER_XY;
	return result;
	}

BLOCK::Type World::getCitySquareBuildingMaterialTypeAt(const IntVector2& blockPos)
	{
	static BLOCK::Type buildingTypes[]=
		{	
		BLOCK::BRICK,BLOCK::GRAY_BRICK,
		BLOCK::SANDSTONE,
		BLOCK::MASONRY,
		BLOCK::WOOD,
		BLOCK::GLASS,
		BLOCK::STONE,
		};
	static int amtBuildingTypes=sizeof(buildingTypes)/sizeof(buildingTypes[0]);
	IntVector2 posInSquareUnits;
	posInSquareUnits.x=blockPos.x>>CITY_SQUARE_BLOCKS_XY_BITS;
	posInSquareUnits.y=blockPos.y>>CITY_SQUARE_BLOCKS_XY_BITS;
	float noise=calcNoise(posInSquareUnits,2);
	int buildingTypeIndex=clampInt((int)(noise*amtBuildingTypes),0,amtBuildingTypes-1);
	return buildingTypes[buildingTypeIndex];
	}

bool World::isCityBlockSidewalkAt(const IntVector2& blockPos)
	{
	static int xs[64]={0};
	static int callCount=0;

	IntVector2 blockPosInSquare=convertWorldBlockPosToCitySquarePos(blockPos);

	if(blockPosInSquare.x<64)
		xs[blockPosInSquare.x]++;
	callCount++;
	while(callCount>1000)
		break;

	int left=CITY_SQUARE_HALF_ROAD_WIDTH;
	int bottom=left;
	int right=CITY_SQUARE_BLOCKS_XY-1-CITY_SQUARE_HALF_ROAD_WIDTH;
	int top=right;

	bool isNotRoad     = blockPosInSquare.x>=left && blockPosInSquare.y>=bottom 
		              && blockPosInSquare.x<=right && blockPosInSquare.y<=top;
	bool canBeSidewalk = blockPosInSquare.x==left || blockPosInSquare.y==bottom 
		              || blockPosInSquare.x==right || blockPosInSquare.y==top;
	bool result = isNotRoad && canBeSidewalk;
	return result;
	}

IntVector2 World::convertWorldBlockPosToCitySquarePos(const IntVector2& blockPos)
	{
	IntVector2 result=blockPos;
	while (result.x==15&&result.y==15)
		break;
	result.x&=CITY_SQUARE_DIMENSION_MASK;
	result.y&=CITY_SQUARE_DIMENSION_MASK;
	return result;
	}

void World::initCities()
	{
	if(m_haveCitiesBeenInitialized/* || m_seed==0*/)
		return;

	for (int cy=0;cy<FINITE_WORLD_COUNTIES_XY;cy++)
	for (int cx=0;cx<FINITE_WORLD_COUNTIES_XY;cx++)
		{
		CountyCoords countyPos(cx,cy);
		m_cities[countyPos]=nullptr;

		IntVector2 cityPosWithinCounty=calcCityPosForCountyAt(countyPos);
		IntVector2 cityWorldPos=getAbsoluteCityPosition(countyPos,cityPosWithinCounty);
		Vector2 heightRange=Chunk::getBlockHeightRange(cityWorldPos.x,cityWorldPos.y,m_seed);
		bool isTooRugged = absval(heightRange.y=heightRange.x)>15;
		if (isTooRugged)
			{
			m_cities[countyPos]=nullptr;
			}
		else
			{
			int extentRadius=LARGE_CITY_BLOCKS_XY/2;
			int verticality=extentRadius;
			float vertScale=calcNoise(cityWorldPos,50);
			vertScale=smoothStartN(vertScale,4);
			verticality=(int)((float)verticality*vertScale);

			City* newcity=new City(cityWorldPos,extentRadius,verticality);
			m_cities[countyPos]=newcity;
			}
		}
	m_cityJumpIter=m_cities.begin();
	m_haveCitiesBeenInitialized=true;

	}

void World::updateActivatedChunks(float /*deltaSeconds*/)
	{
// 	const float CHANGE_TIMER_INTERVAL=0.01f;
// 	static float changeTimer=0.f;
// 	changeTimer+=deltaSeconds;
// 	if(changeTimer<CHANGE_TIMER_INTERVAL)
// 		return;
// 	changeTimer-=CHANGE_TIMER_INTERVAL;
	ChunkCoords centerChunkPos=calcChunkCoordsFromPlayerPos();

	if (m_chunks.size()>=MAX_CHUNKS_LOADED)
		{
		findAndDeactivateFurthestChunk(centerChunkPos);
		return;
		}

	int oldCount=m_chunks.size();
	findAndActivateClosestMissingChunk(centerChunkPos);
	int newCount=m_chunks.size();
	bool didActivate=newCount>oldCount;

	if(didActivate)
		return;

	if (m_chunks.size()>MAX_CHUNKS_LOADED)
		{
		findAndDeactivateFurthestChunk(centerChunkPos);
		return;
		}

	//keepChunkNeighborsUpToDate();
	}

int World::getActiveChunkCount()
	{	
	return m_chunks.size();
	}


void World::processInput(float /*deltaSeconds*/)
	{
	if(g_theInput->wasKeyJustPressed('Z'))
		{
		ChunkCoords chunkCoords=calcChunkCoordsFromCameraPos();
		Chunk* c=getChunkAtCoords(chunkCoords);
		c->debugAddStoneStackAt(*m_cameraPos);
		}
	if (g_theInput->wasKeyJustPressed('X'))  // for debugging purposes - to view the chunk neighbor pointers that seem to be correct
		{
		while(true)
			break;
		}

	}

bool World::gotoNeighboringBlock(ChunkCoords& neighborChunkCoords,IntVector3& neighborBlockCoords,const ChunkCoords& chunkCoords,
								 const IntVector3& blockCoords,const IntVector3& blockDisp)
	{
	bool result=true;
	IntVector3 chunkOffset;

	neighborChunkCoords=chunkCoords;
	neighborBlockCoords=blockCoords+blockDisp;
	IntVector3 blockDispForSingleChunk(CHUNK_BLOCKS_X,CHUNK_BLOCKS_Y,CHUNK_BLOCKS_Z);
	if (Chunk::isOutOfBounds(neighborBlockCoords,chunkOffset) && chunkOffset.z==0)
		{
		neighborChunkCoords=(IntVector2)chunkCoords+IntVector2(chunkOffset.x,chunkOffset.y);
		neighborBlockCoords.x&=X_MASK_BITS_UNSHIFTED;
		neighborBlockCoords.y&=Y_MASK_BITS_UNSHIFTED;
		neighborBlockCoords.z&=Z_MASK_BITS_UNSHIFTED;
		}
	if (!isChunkMissingAt(neighborChunkCoords))
		{
		Chunk* nchunk=getChunkAtCoords(neighborChunkCoords);
		if (nchunk)
			{
			Block* nblock=nchunk->getBlockAddrAt(neighborBlockCoords);
			if (nblock)
				{
				result=true;
				}
			}
		}
	return result;
	}

void World::deactivateAllChunks()
	{
	std::vector<Chunk*> chunksToDelete;
	for (std::map<ChunkCoords,Chunk*>::iterator iter=m_chunks.begin();iter!=m_chunks.end();iter++)
		{
		Chunk* c=iter->second;
		chunksToDelete.push_back(c);
		}

	for (Chunk* c: chunksToDelete)
		{
		if (c)
			deactivateChunk(c);
		}
	chunksToDelete.clear();
	m_chunks.clear();
	}

void World::updateNeighborPointersForChunkAt(const ChunkCoords& chunkCoords)
	{
	Chunk* chunk=getChunkAtCoords(chunkCoords);
	if(chunk==nullptr)
		return;

	ChunkCoords chunkCoordsCopy(chunkCoords);
	ChunkCoords eastCoords=chunkCoordsCopy+ChunkCoords(1,0);
	ChunkCoords westCoords=chunkCoordsCopy-ChunkCoords(1,0);
	ChunkCoords northCoords=chunkCoordsCopy+ChunkCoords(0,1);
	ChunkCoords southCoords=chunkCoordsCopy-ChunkCoords(0,1);

	chunk->m_eastNeighbor = (m_chunks.count(eastCoords)>=1) ? m_chunks[eastCoords] : nullptr;
	chunk->m_westNeighbor = (m_chunks.count(westCoords)>=1) ? m_chunks[westCoords] : nullptr;
	chunk->m_southNeighbor = (m_chunks.count(southCoords)>=1) ? m_chunks[southCoords] : nullptr;
	chunk->m_northNeighbor = (m_chunks.count(northCoords)>=1) ? m_chunks[northCoords] : nullptr;

	if (chunk->m_eastNeighbor)
		chunk->m_eastNeighbor->m_westNeighbor=chunk;
	if (chunk->m_westNeighbor)
		chunk->m_westNeighbor->m_eastNeighbor=chunk;
	if (chunk->m_northNeighbor)
		chunk->m_northNeighbor->m_southNeighbor=chunk;
	if (chunk->m_southNeighbor)
		chunk->m_southNeighbor->m_northNeighbor=chunk;
	}

void World::prepareNeighborPointersForChunkDeactivation(Chunk* /*c*/)
	{
// 	if (c)
// 		{
// 		if (c->m_eastNeighbor)
// 			c->m_eastNeighbor->m_westNeighbor=nullptr;
// 		if (c->m_westNeighbor)
// 			c->m_westNeighbor->m_eastNeighbor=nullptr;
// 		if (c->m_northNeighbor)
// 			c->m_northNeighbor->m_southNeighbor=nullptr;
// 		if (c->m_southNeighbor)
// 			c->m_southNeighbor->m_northNeighbor=nullptr;
// 		}
	}


void World::keepChunkNeighborsUpToDate()
	{
	for (std::pair<ChunkCoords, Chunk*> chunkpair: m_chunks)
		{
		updateNeighborPointersForChunkAt(chunkpair.first);
		}
	}

void World::printCulledChunkPercentage(std::vector<Chunk*>* chunksToRender)
	{
	if(g_theDebugMessagePrinter==nullptr)
		return;
	std::stringstream ss("");
	ss.str("");
	float pctDrawn=((float)chunksToRender->size()/(float)m_chunks.size())*100.f;
	float pctCulled=100.f-pctDrawn;
	ss.precision(3);
	ss<<"Percentage of chunks that were culled from rendering: "<<pctCulled<<"%";
	
	g_theDebugMessagePrinter->addMessage(ss.str(),Rgba::MAGENTA);
	}

void World::addNumberOfCitiesToDebugPrinter()
	{
	std::stringstream ss;
	ss.str("");
	ss<<"Number of cities: ";
	ss<<countCities()<<std::endl;
	ss<<"Estimated number of counties: ";
	ss<<(FINITE_WORLD_COUNTIES_XY*FINITE_WORLD_COUNTIES_XY);
	if(g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->addMessage(ss.str(),Rgba::YELLOW);
	}

int World::countCities()
	{
	int result=0;
	for (int y=0;y<FINITE_WORLD_COUNTIES_XY;y++)
	for (int x=0;x<FINITE_WORLD_COUNTIES_XY;x++)
		{
		if(m_cities[CountyCoords(x,y)]!=nullptr)
		   result++;
		}
	return result;
	}

void World::updateVertexArrays()
	{
	for (std::pair<ChunkCoords,Chunk*> chunkpair: m_chunks)
		{
		Chunk* chunk=chunkpair.second;
		if (chunk->isDirty())
			{
			chunk->rebuildVerts();
			}
		}
	}

void World::updateLighting()
	{
	//TODO: update lighting here
	int dirtyBlocksProcessed=0;
	while (!m_dirtyLightingBlocks.empty())
		{
		// grab and pop off the first block in the queue
		BlockInfo firstDirtyInfo=m_dirtyLightingBlocks.front();
		m_dirtyLightingBlocks.pop_front();
		firstDirtyInfo.clearLightingDirty();
		updateBlockLighting(firstDirtyInfo);
		++dirtyBlocksProcessed;
		}
	while(true)
		break;
	}

void World::updateBlockLighting(const BlockInfo& dirtyBlockInfo)
	{
	BlockInfo biCopy=dirtyBlockInfo;
	//compute my ideal block value

	Block*    myBlock=biCopy.getBlock();
	if(myBlock==nullptr) return;
	Block* nborBlocks[6];
	BlockInfo nborInfos[6];
	if(true)
		{
		int i=0;
		nborInfos[i++]=biCopy.getEast();
		nborInfos[i++]=biCopy.getWest();
		nborInfos[i++]=biCopy.getNorth();
		nborInfos[i++]=biCopy.getSouth();
		nborInfos[i++]=biCopy.getUp();
		nborInfos[i++]=biCopy.getDown();

		for (i=0;i<6;i++)
			{
			nborBlocks[i]=nullptr;
			nborBlocks[i]=nborInfos[i].getBlock();
			}
		}

	int idealLight=myBlock->getDef().m_lightEmitted;
	if (!myBlock->isOpaque())
		{
		for (int i=0;i<6;i++)
			{
			// 		bool isOpaque = nborBlocks[i] ? nborBlocks[i]->isOpaque() : false;
			if (nborBlocks[i] /*&& !nborBlocks[i]->isOpaque()*/)
				{
				int curLight=nborBlocks[i]->getLight()-LIGHT_STEP;
				if (nborBlocks[i]->isSky())
					curLight=maxOf(curLight,getCurrentOutdoorLight());
				idealLight=maxOf(idealLight,curLight);
				}
			}
		}

	idealLight=max(idealLight,DARKEST_POSSIBLE_LIGHT);

	//return if already correct
	if(idealLight==myBlock->getLight())
		return;
	//set light value to ideal
	myBlock->setLight(idealLight);
	//dirty non-opaque neighbors
	for (int i=0;i<6;i++)
		{
		if (nborBlocks[i] && !nborBlocks[i]->isOpaque()/* && !nborBlocks[i]->isSky()*/)
			{
			if (!nborBlocks[i]->isLightDirty())
				{
				enqueueDirtyLightBlock(nborInfos[i]);
				}
			}
		}
	// clear the block's LIGHTINT_DIRTY bitflag.
	myBlock->setLightDirtyBit(false);
	// set the dirty flag for the chunk
	biCopy.m_chunk->m_isVboDirty=true;
	biCopy.m_chunk->makeNeighborsDirty();
	}

void World::updateEntities(float deltaSeconds)
	{
	m_thePlayer->update(deltaSeconds);
	detectAndCorrectCollisions(deltaSeconds);
	}

void World::detectAndCorrectCollisions(float deltaSeconds)
	{
	m_thePlayer->detectAndCorrectCollisions(deltaSeconds);
	}

void World::enqueueDirtyLightBlock(const BlockInfo& dirtyBlockInfo)
	{
	Block* dirtyBlock = dirtyBlockInfo.getBlock();
	if(dirtyBlock==nullptr)
		return;
	dirtyBlock->setLightDirtyBit(true);
	while(dirtyBlockInfo.m_chunk->m_eastNeighbor==(Chunk*)0xdddddddd)
		break;
	m_dirtyLightingBlocks.push_back(dirtyBlockInfo);
	}

int World::getCurrentOutdoorLight()
	{
	if(false)
		return LIGHT_LEVEL_MOONLIGHT;
	else
		return LIGHT_LEVEL_SUNLIGHT;
	}

IntVector3 World::calcWorldPos(const ChunkCoords& chunkPos,const IntVector3& blockPos)
	{
	IntVector3 result;
	result.x=chunkPos.x*CHUNK_BLOCKS_X+blockPos.x;
	result.y=chunkPos.y*CHUNK_BLOCKS_Y+blockPos.y;
	result.z=blockPos.z&Z_MASK_BITS_UNSHIFTED;	
	return result;
	}

BlockInfo World::getBlockInfoAtWorldPos(const Vector3& absWorldPos)
	{
	BlockInfo result;
	IntVector3 localPos=IntVector3((int)floor(absWorldPos.x),(int)floor(absWorldPos.y),(int)floor(absWorldPos.z));
	localPos.x &= X_MASK_BITS_UNSHIFTED;
	localPos.y &= Y_MASK_BITS_UNSHIFTED;
	localPos.z &= Z_MASK_BITS_UNSHIFTED;

	ChunkCoords chunkCoords=getChunkCoordsFromWorldPos(absWorldPos);
	Chunk* chunk=getChunkAtCoords(chunkCoords);

	result.m_chunk=chunk;
	if(chunk)
		result.m_blockIndex=chunk->getBlockIndexForLocalCoords(localPos);
	return result;
	}

Block* World::getBlockAt(const IntVector3& absBlockPos)
	{
	IntVector3 blockPosInChunk=IntVector3(absBlockPos.x&X_MASK_BITS_UNSHIFTED,absBlockPos.y&Y_MASK_BITS_UNSHIFTED,absBlockPos.z);
	Chunk* chunk=getChunkAtWorldBlockCoords(absBlockPos);
	if(chunk==nullptr)
		return nullptr;
	return chunk->getBlockAddrAt(blockPosInChunk);
	}

Chunk* World::getChunkAtWorldBlockCoords(const IntVector3& absBlockPos)
	{
	ChunkCoords chunkPos=ChunkCoords(absBlockPos.x>>CHUNK_X_BITS,absBlockPos.y>>CHUNK_Y_BITS);
	Chunk* chunk=getChunkAtCoords(chunkPos);

	return chunk;
	}

ChunkCoords World::calcClosestMissingChunkOutOfTo(const std::vector<ChunkCoords>& missingChunks, const ChunkCoords& centerChunkCoords)
	{
	assert(missingChunks.size()>0);
	int closestDistSqr=centerChunkCoords.calcDistanceSquaredTo(missingChunks.at(0));
	ChunkCoords result=missingChunks.at(0);
	for (int i=1;i<(int)missingChunks.size();i++)
		{
		ChunkCoords curChunkPos=missingChunks.at(i);
		int curDistSqr=centerChunkCoords.calcDistanceSquaredTo(curChunkPos);
		if (curDistSqr<closestDistSqr)
			{
			closestDistSqr=curDistSqr;
			result=curChunkPos;
			}
		}
	return result;
	}

ChunkCoords World::findClosestMissingChunkTo(const ChunkCoords& centerChunkPos)
	{
	std::vector<ChunkCoords> missingChunks;
	int ipass=0;
	int ispan=0;
	int spanlen=1;
	if (isChunkMissingAt(centerChunkPos))
		missingChunks.push_back(centerChunkPos);
	ChunkCoords chunkPos(centerChunkPos);
	ChunkInc chunkInc(0,1);
	chunkPos+=chunkInc;
	while (missingChunks.size()<100)
		{
		if(isChunkMissingAt(chunkPos))
			missingChunks.push_back(chunkPos);
		ispan++;
		if (ispan>=spanlen)
			{
			ispan=0;
			chunkInc.rotate90DegreesLeft();
			ipass++;
			if (ipass>=2)
				{
				ipass=0;
				++spanlen;
				}
			}
		chunkPos+=chunkInc;
		}
	return calcClosestMissingChunkOutOfTo(missingChunks, centerChunkPos);
	}

bool World::isChunkMissingAt(const ChunkCoords& potentialMissingChunkCoords)
	{
	bool result = true;
	if(m_chunks.count(potentialMissingChunkCoords)==1)
		result=false;
	return result;
	}

RaycastResult World::raycast(const Vector3& start,const Vector3& dir,float len, bool shouldTraceRay/*=true*/)
	{

	//TODO: fix so that casts do not register hits at "Z roof" of world.
	RaycastResult result;
	const int AMT_STEPS = 1000;
	const float STEP_LEN_FACTOR = 1.f/(float)AMT_STEPS;
	float stepLen=len*STEP_LEN_FACTOR;

	Vector3 curPos=start;
	Vector3 prevPos=start;
	for (int step=0;step<AMT_STEPS;step++)
		{
		curPos=start+dir*stepLen*(float)step;
		if(shouldTraceRay)
			g_theGame->addDebugDotAt(curPos);
		if (isWorldPositionInSolid(curPos))
			{
			result.m_impactPos=curPos;
			ChunkCoords chunkCoords=getChunkCoordsFromWorldPos(curPos);
			result.m_impactChunk=getChunkAtCoords(chunkCoords);
			result.m_impactFraction=(float)step/(float)AMT_STEPS;
			if (result.m_impactChunk)
				{
				result.m_impactLocalCoords = result.m_impactChunk->convertWorldCoordsToBlockCoords3(curPos);
				result.m_blockIndex = result.m_impactChunk->getBlockIndexForLocalCoords(result.m_impactLocalCoords);
				}
			else
				{
				result.m_impactLocalCoords = IntVector3(0,0,0);
				result.m_blockIndex = 0;
				}
			AABB3 blockBounds=result.m_impactChunk->calcBlockBoundsInWorld(result.m_impactLocalCoords);
			LineSegment3 rayseg(prevPos,curPos);
			result.m_impactFaceNormal=blockBounds.getFaceIntersectionNormal(rayseg);
			return result;
			}
		prevPos=curPos;
		}
	return result;
	}

bool World::isWorldPositionInSolid(const Vector3& pos)
	{
	BlockInfo blockInfo=getBlockInfoForWorldPos(pos);
	if(blockInfo.m_chunk==nullptr)
		return false;
	const Block& block = blockInfo.m_chunk->m_blocks[blockInfo.m_blockIndex];
	return block.isSolid();
	}

Chunk* World::getChunkForWorldPos(const Vector3& pos)
	{
	Chunk* result=nullptr;
	if(pos.z<0.f || pos.z>CHUNK_BLOCKS_Z*BLOCK_CUBE_SIZE)
		return result;
	ChunkCoords chunkCoords = ChunkCoords((int)floor(pos.x),(int)floor(pos.y));
	result=getChunkAtCoords(chunkCoords);
	return result;
	}

BlockInfo World::getBlockInfoForWorldPos(const Vector3& pos)
	{
	ChunkCoords chunkPos=getChunkCoordsFromWorldPos(pos);
	Chunk* chunk=getChunkAtCoords(chunkPos);
	BlockInfo result;
	result.m_chunk=chunk;
	result.m_blockIndex=-1;
	if (chunk==nullptr)
		return result;
	//TODO: fill this block info function out when you get around to raycasting
	IntVector3 blockPosWithinChunk=chunk->convertWorldCoordsToBlockCoords3(pos);
	result.m_blockIndex=chunk->getBlockIndexForLocalCoords(blockPosWithinChunk);
	return result;
	}

