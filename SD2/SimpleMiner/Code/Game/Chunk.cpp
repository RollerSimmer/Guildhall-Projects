#include "Game/Chunk.hpp"
#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/File/FileUtils.hpp"
#include "Engine/Math/Noise.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/Game.hpp"
#include <sstream>

const IntVector3 IntVector3::ZERO(0, 0, 0);

Chunk::Chunk(const IntVector2& chunkcoords)
	: m_chunkCoords(chunkcoords)
	, m_isVboDirty(true)
	, m_wasChanged(false)
	, m_eastNeighbor(nullptr)
	, m_westNeighbor(nullptr)
	, m_northNeighbor(nullptr)
	, m_southNeighbor(nullptr)
	, m_vboID(0)
	, m_amtVerticesInVbo(0)
	{
	while (true)
		break;
	m_verts.reserve(AMT_FACES_PER_BLOCK);
// 	populateBlocks();
	}

Chunk::Chunk()
	: m_chunkCoords(0,0)
	, m_isVboDirty(true)
	, m_wasChanged(false)
	, m_eastNeighbor(nullptr)
	, m_westNeighbor(nullptr)
	, m_northNeighbor(nullptr)
	, m_southNeighbor(nullptr)
	, m_vboID(0)
	, m_amtVerticesInVbo(0)
	{
	while(true)
		break;
	m_verts.reserve(AMT_FACES_PER_BLOCK);
	}

Chunk::~Chunk()
	{
	setEastNeighbor(nullptr);
	setWestNeighbor(nullptr);
	setSouthNeighbor(nullptr);
	setNorthNeighbor(nullptr);	
	}

void Chunk::setWestNeighbor(Chunk* nbor)
	{
	if (m_westNeighbor)
		m_westNeighbor->m_eastNeighbor=nullptr;
	m_westNeighbor=nbor;
	if (m_westNeighbor)
		m_westNeighbor->m_eastNeighbor=this;
	}

void Chunk::setEastNeighbor(Chunk* nbor)
	{
	if (m_eastNeighbor)
		m_eastNeighbor->m_westNeighbor=nullptr;
	m_eastNeighbor=nbor;
	if (m_eastNeighbor)
		m_eastNeighbor->m_westNeighbor=this;
	}

void Chunk::setSouthNeighbor(Chunk* nbor)
	{
	if (m_southNeighbor)
		m_southNeighbor->m_northNeighbor=nullptr;
	m_southNeighbor=nbor;
	if (m_southNeighbor)
		m_southNeighbor->m_northNeighbor=this;
	}

void Chunk::setNorthNeighbor(Chunk* nbor)
	{
	if (m_northNeighbor)
		m_northNeighbor->m_southNeighbor=nullptr;
	m_northNeighbor=nbor;
	if (m_northNeighbor)
		m_northNeighbor->m_southNeighbor=this;
	}

void Chunk::update(float /*deltaSeconds*/)
	{
	// nothing for now
	
	}

void Chunk::render() const
	{
// 	if(m_verts.size()==0)
// 		return;
	if(m_vboID==0)
		return;

// 	DebuggerPrintf("number of verts: %d\n",m_verts.size());
	Vector3 worldpos = calcMyPositionInWorld();
	g_theRenderer->pushGraphicsMatrix();
	g_theRenderer->translate3D(worldpos);

	//horizon scale
	if (g_doHorizonEffect)
		{
		Vector3 camPos=g_theGame->getCamera().m_pos;
		Vector3 disp=worldpos-camPos;
		camPos.z=disp.z=0.f;
		float dist=disp.calcLength();
		float maxDist=(float)(CHUNK_VISIBILITY_DISTANCE*CHUNK_BLOCKS_X);
		float zScale=(maxDist-dist)/maxDist;
		zScale=clampFloat(zScale,0.f,0.5f)*(1.f/0.5f);
		zScale=zScale*zScale*zScale;

// 		zScale=zScale*zScale*zScale;
		float hScale=1.f;
		Vector3 scalingFactors(hScale,hScale,zScale);
		g_theRenderer->scale3D(scalingFactors);
		}

	g_theRenderer->drawVbo(m_vboID,m_amtVerticesInVbo);

// 	g_theRenderer->drawQuads3D(m_verts);

	drawDebugChunkBounds();
	g_theRenderer->popGraphicsMatrix();
	}

void Chunk::drawDebugChunkBounds() const
{
	float width=(float)CHUNK_BLOCKS_X*(float)BLOCK_CUBE_SIZE;
	float depth=(float)CHUNK_BLOCKS_Y*(float)BLOCK_CUBE_SIZE;
	float height=(float)CHUNK_BLOCKS_Z*(float)BLOCK_CUBE_SIZE;

	Vector3 mins(0.f,0.f,0.f);
	Vector3 maxs(width,depth,height);
	AABB3 boxBounds(mins,maxs);

	Rgba color=Rgba::MAGENTA;
	color.setAlphaAsFloat(0.5f);

	g_theRenderer->drawFilledBox3(boxBounds,color);
	}

void Chunk::populateBlocks()
	{

	for (int z=0;z<CHUNK_BLOCKS_Z;z++)
	for (int y=0;y<CHUNK_BLOCKS_Y;y++)
	for (int x=0;x<CHUNK_BLOCKS_X;x++)
		{
		genBlockAt(IntVector3(x,y,z));
		}
	//rebuildVerts();
	}

void Chunk::rebuildVerts()
	{
	DebuggerPrintf("Rebuild at ChunkCoords: (%d,%d)\n",m_chunkCoords.x,m_chunkCoords.y);

	m_verts.clear();
	for (int z=0;z<CHUNK_BLOCKS_Z;z++)
	for (int y=0;y<CHUNK_BLOCKS_Y;y++)
	for (int x=0;x<CHUNK_BLOCKS_X;x++)
		{	
		rebuildVertsForSingleBlock(IntVector3(x,y,z));
		}
	m_amtVerticesInVbo=m_verts.size();

	bindVertsToVbo();
	m_isVboDirty=false;
	m_verts.clear();
	}

void Chunk::rebuildVertsForSingleBlock(const IntVector3& pos)
	{
	Block& block=*getBlockAddrAt(pos);
	BlockDefinition blockdef=block.getDef();
	BlockInfo blockInfo(this, getBlockIndexForLocalCoords(pos));
	if(!blockdef.m_isOpaque) 
		return;

	BlockInfo upInfo=blockInfo.getUp();
	BlockInfo downInfo=blockInfo.getDown();
	BlockInfo eastInfo=blockInfo.getEast();
	BlockInfo westInfo=blockInfo.getWest();
	BlockInfo southInfo=blockInfo.getSouth();
	BlockInfo northInfo=blockInfo.getNorth();

	AABB2 texcoords;
	float& texleft   = texcoords.m_mins.x;
	float& texright  = texcoords.m_maxs.x;
	float& textop    = texcoords.m_maxs.y;
	float& texbottom = texcoords.m_mins.y;

	AABB3 bounds=calcBlockBoundsInChunk(pos);	
	float& xlo=bounds.m_mins.x;
	float& xhi=bounds.m_maxs.x;
	float& ylo=bounds.m_mins.y;
	float& yhi=bounds.m_maxs.y;
	float& zlo=bounds.m_mins.z;
	float& zhi=bounds.m_maxs.z;

	Rgba faceColor;

	// bottom face
	if (downInfo.m_chunk!=blockInfo.m_chunk || !doesNeighborBlockObstructView(downInfo))
		{
		faceColor=downInfo.getFaceColorFromLightValue();
		texcoords=blockdef.m_bottomTexCoords;
		m_verts.push_back(Vertex3(Vector3(xhi,yhi,zlo),faceColor,Vector2(texleft,texbottom)));
		m_verts.push_back(Vertex3(Vector3(xhi,ylo,zlo),faceColor,Vector2(texleft,textop)));
		m_verts.push_back(Vertex3(Vector3(xlo,ylo,zlo),faceColor,Vector2(texright,textop)));
		m_verts.push_back(Vertex3(Vector3(xlo,yhi,zlo),faceColor,Vector2(texright,texbottom)));
		}

	// top face
	if (upInfo.m_chunk!=blockInfo.m_chunk || !doesNeighborBlockObstructView(upInfo))
		{
		faceColor=upInfo.getFaceColorFromLightValue();
		texcoords=blockdef.m_topTexCoords;
		m_verts.push_back(Vertex3(Vector3(xlo,yhi,zhi),faceColor,Vector2(texleft,texbottom)));
		m_verts.push_back(Vertex3(Vector3(xlo,ylo,zhi),faceColor,Vector2(texleft,textop)));
		m_verts.push_back(Vertex3(Vector3(xhi,ylo,zhi),faceColor,Vector2(texright,textop)));
		m_verts.push_back(Vertex3(Vector3(xhi,yhi,zhi),faceColor,Vector2(texright,texbottom)));
		}

	float eastAndWestShadowFactor=0.9f;

	// east face 
	if (eastInfo.m_chunk!=blockInfo.m_chunk || !doesNeighborBlockObstructView(eastInfo))
		{
		faceColor=eastInfo.getFaceColorFromLightValue().getRGBScaledBy(eastAndWestShadowFactor);
		texcoords=blockdef.m_sideTexCoords;
		m_verts.push_back(Vertex3(Vector3(xhi,ylo,zhi),faceColor,Vector2(texleft,texbottom)));
		m_verts.push_back(Vertex3(Vector3(xhi,ylo,zlo),faceColor,Vector2(texleft,textop)));
		m_verts.push_back(Vertex3(Vector3(xhi,yhi,zlo),faceColor,Vector2(texright,textop)));
		m_verts.push_back(Vertex3(Vector3(xhi,yhi,zhi),faceColor,Vector2(texright,texbottom)));
		}

	// west face 
	if (westInfo.m_chunk!=blockInfo.m_chunk || !doesNeighborBlockObstructView(westInfo))
		{
		faceColor=westInfo.getFaceColorFromLightValue().getRGBScaledBy(eastAndWestShadowFactor);
		texcoords=blockdef.m_sideTexCoords;
		m_verts.push_back(Vertex3(Vector3(xlo,yhi,zhi),faceColor,Vector2(texleft,texbottom)));
		m_verts.push_back(Vertex3(Vector3(xlo,yhi,zlo),faceColor,Vector2(texleft,textop)));
		m_verts.push_back(Vertex3(Vector3(xlo,ylo,zlo),faceColor,Vector2(texright,textop)));
		m_verts.push_back(Vertex3(Vector3(xlo,ylo,zhi),faceColor,Vector2(texright,texbottom)));
		}

	// north and south shadow factor
	float northAndSouthShadowFactor=eastAndWestShadowFactor*eastAndWestShadowFactor*eastAndWestShadowFactor;

	// south face
	if (southInfo.m_chunk!=blockInfo.m_chunk || !doesNeighborBlockObstructView(southInfo))
		{
		faceColor=southInfo.getFaceColorFromLightValue().getRGBScaledBy(northAndSouthShadowFactor);
		texcoords=blockdef.m_sideTexCoords;
		m_verts.push_back(Vertex3(Vector3(xlo,ylo,zhi),faceColor,Vector2(texleft,texbottom)));
		m_verts.push_back(Vertex3(Vector3(xlo,ylo,zlo),faceColor,Vector2(texleft,textop)));
		m_verts.push_back(Vertex3(Vector3(xhi,ylo,zlo),faceColor,Vector2(texright,textop)));
		m_verts.push_back(Vertex3(Vector3(xhi,ylo,zhi),faceColor,Vector2(texright,texbottom)));
		}

	// north face 
	if (northInfo.m_chunk!=blockInfo.m_chunk || !doesNeighborBlockObstructView(northInfo))
		{
		faceColor=northInfo.getFaceColorFromLightValue().getRGBScaledBy(northAndSouthShadowFactor);
		texcoords=blockdef.m_sideTexCoords;
		m_verts.push_back(Vertex3(Vector3(xhi,yhi,zhi),faceColor,Vector2(texleft,texbottom)));
		m_verts.push_back(Vertex3(Vector3(xhi,yhi,zlo),faceColor,Vector2(texleft,textop)));
		m_verts.push_back(Vertex3(Vector3(xlo,yhi,zlo),faceColor,Vector2(texright,textop)));
		m_verts.push_back(Vertex3(Vector3(xlo,yhi,zhi),faceColor,Vector2(texright,texbottom)));
		}
	}

void Chunk::bindVertsToVbo()
	{
	if(!m_isVboDirty)
		return;
	if (m_vboID!=0)
		{
		g_theRenderer->unbindBuffer(m_vboID);
		m_vboID=0;
		}
	m_vboID=g_theRenderer->getBoundedVbo(m_verts);
	m_isVboDirty=false;
	}

void Chunk::genBlockAt(const IntVector3& pos, int height/*=GRASS_LEVEL*/, BLOCK::Type surfaceType/*=BLOCK::GRASS*/)
	{

	if (pos.z> height)
		{
		if(pos.z<=g_waterLevel)
			addBlockOfTypeAt(BLOCK::WATER,pos);
		else
			addBlockOfTypeAt(BLOCK::AIR,pos);
		}
	else if (pos.z==g_waterLevel)
		{
		addBlockOfTypeAt(BLOCK::SAND,pos);
		}
	else if (pos.z==height)
		{
		addBlockOfTypeAt(surfaceType,pos);
		}
	else if (pos.z>=DIRT_LEVEL)
		{
		addBlockOfTypeAt(BLOCK::DIRT,pos);
		}
	else //if (z>=STONE_LEVEL)
		{
		addBlockOfTypeAt(BLOCK::STONE,pos);
		}
	}

void Chunk::addBlockStackAt(const IntVector3& pos,BLOCK::Type surfaceType)
	{
	IntVector3 blockpos=pos;
	int height = pos.z;

	//TODO: place road code here.
	bool shouldBuildBuilding=false;
	BLOCK::Type buildingOverride;
	int buildingTopZ;
	bool shouldOverrideSurface=false;
	BLOCK::Type surfaceOverride;

	IntVector2 blockAbsPos=IntVector2(pos.x,pos.y);
	IntVector2 squareCenterPos=g_theWorld->calcCitySquareCenterAt(blockAbsPos);
	int surfaceHeight=g_theWorld->getDefaultSurfaceHeightAt(blockAbsPos);
	int developmentHeightAboveGround=g_theWorld->getCityDevelopmentHeightAt(squareCenterPos);
// 	developmentHeight=20;
	if (developmentHeightAboveGround>1)
		{
		if (g_theWorld->isCityBlockSidewalkAt(blockAbsPos))
			{
			shouldOverrideSurface=true;
			surfaceOverride=BLOCK::MASONRY;
			}
		else if (g_theWorld->isCityBlockRoadAt(blockAbsPos))
			{
			shouldOverrideSurface=true;
			surfaceOverride=BLOCK::ASPHALT;
			}
		else if(surfaceHeight>g_waterLevel)
			{
			shouldBuildBuilding=true;
			buildingOverride=g_theWorld->getCitySquareBuildingMaterialTypeAt(blockAbsPos);
			buildingTopZ=developmentHeightAboveGround-1+g_theWorld->getBuildingFoundationHeightAt(blockAbsPos);
			if(buildingTopZ>CHUNK_BLOCKS_Z-3) buildingTopZ=CHUNK_BLOCKS_Z-3;
			}

		}

	BLOCK::Type usedSurfaceType=shouldOverrideSurface? surfaceOverride: surfaceType;

	for (int z=0;z<CHUNK_BLOCKS_Z;z++)
		{
		blockpos.z=z;
		if (shouldBuildBuilding && z>=surfaceHeight && z<=buildingTopZ)
			addBlockOfTypeAt(buildingOverride,blockpos);
		else
			genBlockAt(blockpos,height,usedSurfaceType);
		}
	}

AABB3 Chunk::calcBlockBoundsInChunk(const IntVector3& blockPos)
	{
	AABB3 result;
	result.m_mins.x=(float)blockPos.x*(float)BLOCK_CUBE_SIZE;
	result.m_mins.y=(float)blockPos.y*(float)BLOCK_CUBE_SIZE;
	result.m_mins.z=(float)blockPos.z*(float)BLOCK_CUBE_SIZE;
	result.m_maxs.x=result.m_mins.x+(float)BLOCK_CUBE_SIZE;
	result.m_maxs.y=result.m_mins.y+(float)BLOCK_CUBE_SIZE;
	result.m_maxs.z=result.m_mins.z+(float)BLOCK_CUBE_SIZE;
	return result;
	}

AABB3 Chunk::calcBlockBoundsInWorld(const IntVector3& blockPos)
	{
	AABB3 result;
	Vector3 chunkpos=calcMyPositionInWorld();
	result.m_mins.x=(float)blockPos.x*BLOCK_CUBE_SIZE;
	result.m_mins.y=(float)blockPos.y*BLOCK_CUBE_SIZE;
	result.m_mins.z=(float)blockPos.z*BLOCK_CUBE_SIZE;
	result.m_mins+=chunkpos;
	result.m_maxs.x=result.m_mins.x+BLOCK_CUBE_SIZE;
	result.m_maxs.y=result.m_mins.y+BLOCK_CUBE_SIZE;
	result.m_maxs.z=result.m_mins.z+BLOCK_CUBE_SIZE;
	return result;
	}

AABB3 Chunk::calcChunkBoundsInWorld()
	{
	AABB3 result(calcBlockBoundsInWorld(IntVector3(0,0,0)).m_mins,
				 calcBlockBoundsInWorld(IntVector3(CHUNK_BLOCKS_X,CHUNK_BLOCKS_Y,CHUNK_BLOCKS_Z)).m_mins);
	return result;		       
	}

Block* Chunk::getBlockAddrByIndex(int blockIndex)
	{
	if (blockIndex<0 || blockIndex>=BLOCKS_PER_CHUNK)
		return nullptr;
	return &m_blocks[blockIndex];
	}

void Chunk::populateFromNoise(unsigned int seed)
{
	//TODO: populate all blocks in chunk from deterministic noise function.
	for(int iy=0;iy<CHUNK_BLOCKS_Y;iy++)
	for(int ix=0;ix<CHUNK_BLOCKS_X;ix++)
		{
		int x = ix + this->m_chunkCoords.x*CHUNK_BLOCKS_X;
		int y = iy + this->m_chunkCoords.y*CHUNK_BLOCKS_Y;
		int z = getNoiseHeightAt(x,y,seed);
		addBlockStackAt(IntVector3(x,y,z),calcBiomeSurfaceType(x,y,z,seed));
		// TODO: activate the following line after getting the noise to work.
		// int z = getNoiseHeightAt(x,y);
		}
	}

// ensures that more flat areas are generated than mountainous areas

int Chunk::flattenNoise(int rawNoise)
	{
	float heightRatio=((float)rawNoise+0.5f)/(float)CHUNK_BLOCKS_Z;
	float fresultRatio=/*smoothStepN(*/heightRatio/*,2,2)*/;
	float fresult=fresultRatio*(float)CHUNK_BLOCKS_Z;
	int result=(int)fresult;
	return result;
	}

int Chunk::getRawHeightAt(int x,int y, unsigned int seed)
	{
	const float PERLIN_BOX_SIZE=40.f;
	float fx=(float)x;
	float fy=(float)y;
	float scale=PERLIN_BOX_SIZE;
	int numOctaves=10;
	float octavePersistence=0.3f;
	float octaveScale=2.5;
	bool shouldRenormalize=true;
	float fnoise = Compute2dPerlinNoise( fx, fy, scale, numOctaves, octavePersistence, octaveScale, shouldRenormalize, seed );
	fnoise+=1.f;
	fnoise*=0.5f;
	Vector2 heightRange=getBlockHeightRange(x,y,seed);
	float rangedNoise=fnoise*(heightRange.y-heightRange.x);
	rangedNoise+=heightRange.x;
	float fz=rangedNoise*(float)(CHUNK_BLOCKS_Z)*MAX_GENERATED_HEIGHT_PCT;
	int result=(int)fz;
	return result;
	}

float Chunk::getBlockMoisture(int x,int y,unsigned int seed)
	{
	seed+=500000;
	const float PERLIN_BOX_SIZE=400.f;
	float fx=(float)x;
	float fy=(float)y;
	float scale=PERLIN_BOX_SIZE;
	int numOctaves=5;
	float octavePersistence=0.3f;
	float octaveScale=2.5;
	bool shouldRenormalize=true;
	float fnoise = Compute2dPerlinNoise(fx,fy,scale,numOctaves,octavePersistence,octaveScale,shouldRenormalize,seed);
	fnoise+=1.f;
	fnoise*=0.5f;
	float result=(float)fnoise;
	return result;
	}

Vector2 Chunk::getBlockHeightRange(int x,int y,unsigned int seed)
	{
	seed+=1000000;
	float baseBoxSize=200.f;
	float peakBoxSize=50.f;
	float fx=(float)x;
	float fy=(float)y;

	int numOctaves=5;
	float octavePersistence=0.3f;
	float octaveScale=2.5;
	bool shouldRenormalize=true;
	float fnoise;

	float scale=baseBoxSize;
	fnoise = Compute2dPerlinNoise(fx,fy,scale,numOctaves,octavePersistence,octaveScale,shouldRenormalize,seed);
	fnoise+=1.f;
	fnoise*=0.5f;
	float baseScale=fnoise;
	baseScale*=0.2f;
	baseScale+=0.3f;

	scale=peakBoxSize;
	fnoise = Compute2dPerlinNoise(fx,fy,scale,numOctaves,octavePersistence,octaveScale,shouldRenormalize,seed);
	fnoise+=1.f;
	fnoise*=0.5f;
	fnoise*=fnoise;
	float peakScale=fnoise;
	peakScale*=0.2f;
	peakScale+=0.4f;
	peakScale=maxOf(peakScale,baseScale);

	Vector2 result(baseScale,peakScale);
	return result;
	}

float Chunk::getBaseTemperatureForBlock(int x,int y)
	{
	UNUSED(x);
	float result;
	float fy=(float)y;
	float maxLatitude=(float)(FINITE_WORLD_AMT_CHUNKS_Y*CHUNK_BLOCKS_Y);
	result=(maxLatitude-fy)/maxLatitude;
	return result;
	}

void Chunk::debugAddStoneStackAt(const Vector3& pos)
	{
	IntVector3 stackpos;
	IntVector2 tilepos=convertWorldCoordsToBlockCoords(Vector2(pos));
	for (int z=0;z<CHUNK_BLOCKS_Z;z++)
		{
		stackpos=IntVector3(tilepos.x,tilepos.y,z);
		Block* curblock=getBlockAddrAt(stackpos);
		if (!curblock->isSolid())
			{
			curblock->setType(BLOCK::GRAY_BRICK);
			}
		}
	m_isVboDirty=true;
	m_wasChanged=true;
	}

bool Chunk::populateFromFile()
	{

	std::vector<unsigned char> encodedBuffer;
	std::vector<unsigned char> buffer;
	std::string myFileName=buildMyFileName();
	if(!readBufferFromFile(encodedBuffer,myFileName))
		return false;
	if(!decodeFileBuffer(buffer,encodedBuffer))
		return false;
	if(!populateFromRawData(buffer))
		return false;

	return true;
	}


void Chunk::saveToFile()
	{
	std::vector<unsigned char> buffer;
	std::vector<unsigned char> encodedBuffer;
	buildFileBufferUnencoded(buffer);
	encodeFileBuffer(encodedBuffer,buffer);
	std::string myFileName=buildMyFileName();
	writeBufferToFile(encodedBuffer,myFileName);
	}

void Chunk::buildFileBufferUnencoded(std::vector<unsigned char>& buffer)
	{
	if(buffer.size()>0) buffer.clear();
	for (int z=0;z<CHUNK_BLOCKS_Z;z++)
	for (int y=0;y<CHUNK_BLOCKS_Y;y++)
	for (int x=0;x<CHUNK_BLOCKS_X;x++)
		{
		Block* b=getBlockAddrAt(IntVector3(x,y,z));
		buffer.push_back((unsigned char)b->m_type);
		}
	}

void Chunk::encodeFileBuffer(std::vector<unsigned char>& encodedBuffer,const std::vector<unsigned char>& buffer)
	{
	ChunkFileHeader header;
	header.m_version=CURRENT_CHUNK_VERSION;
	header.m_bitCountX=(unsigned char)CHUNK_X_BITS;
	header.m_bitCountY=(unsigned char)CHUNK_Y_BITS;
	header.m_bitCountZ=(unsigned char)CHUNK_Z_BITS;
	unsigned char* headerBytes=header.asArray();
	for (int i=0;i<4;i++)
		encodedBuffer.push_back(headerBytes[i]);

	int repeatCount=0;
	bool isFirstByte=true;
	unsigned char byteToWrite=0xff;
	for (unsigned char b: buffer)
		{
		if (isFirstByte)
			{
			repeatCount=1;
			byteToWrite=b;
			isFirstByte=false;
			}
		else if (b!=byteToWrite || repeatCount==255)
			{
			encodedBuffer.push_back(byteToWrite);
			encodedBuffer.push_back((unsigned char)repeatCount);
			repeatCount=1;
			byteToWrite=b;
			}
		else if (b==byteToWrite)
			{
			++repeatCount;
			}
		}
	encodedBuffer.push_back(byteToWrite);
	encodedBuffer.push_back((unsigned char)repeatCount);
	}

bool Chunk::isOutOfBounds(const IntVector3& blockCoords,IntVector3& chunkOffset)
	{
	chunkOffset=IntVector3(0,0,0);
	bool result=false;
	
	int maxX=CHUNK_BLOCKS_X-1;
	int maxY=CHUNK_BLOCKS_Y-1;
	int maxZ=CHUNK_BLOCKS_Z-1;
	
	if (blockCoords.x<0 || blockCoords.y<0 || blockCoords.z<0 || blockCoords.x>maxX || blockCoords.y>maxY || blockCoords.z>maxZ)
		result=true;

	chunkOffset.x=blockCoords.x/CHUNK_BLOCKS_X;
	chunkOffset.y=blockCoords.y/CHUNK_BLOCKS_Y;
	chunkOffset.z=blockCoords.z/CHUNK_BLOCKS_Z;
	if(blockCoords.x<0) chunkOffset.x--;
	if(blockCoords.y<0) chunkOffset.y--;
	if(blockCoords.z<0) chunkOffset.z--;

	return result;
	}

void Chunk::identifySkyBlocks()
	{
	for(int y=0;y<CHUNK_BLOCKS_Y;y++)
	for (int x=0;x<CHUNK_BLOCKS_X;x++)
		{
		identifySkyBlocksForColumn(x,y);
		}
	}

void Chunk::identifySkyBlocksForColumn(int x,int y)
	{
	clearSkyBitsForColumn(x, y);
	IntVector3 pos(x,y,maxOf(CHUNK_BLOCKS_Z-1,0));
	BlockInfo bi(this,getBlockIndexForLocalCoords(pos));
	Block* curblock=bi.getBlock();
	bool wasSolidFound=false;
	bool isDone = false;
	while (!isDone)
		{
		isDone=curblock==nullptr;
		if (!isDone)
			{
			wasSolidFound=curblock->isSolid();
			if (!wasSolidFound)
				{
				curblock->setSkyBit(true);
				}
			}
		isDone=isDone||wasSolidFound;
		bi.moveDown();
		curblock=bi.getBlock();
		}
	}

void Chunk::clearSkyBitsForColumn(int x,int y)
	{
	IntVector3 pos(x,y,maxOf(CHUNK_BLOCKS_Z-1,0));
	BlockInfo bi(this,getBlockIndexForLocalCoords(pos));
	Block*curblock=bi.getBlock();
	while (curblock)
		{
		curblock->setSkyBit(false);
		bi.moveDown();
		curblock=bi.getBlock();
		}
	}

void Chunk::initializeLighting()
	{
	resetAllLightValuesToZero();
	initializeLightAndDirtyForSkyBlocks();
	initLightAndDirtyForSelfLightedBlocks();
	enqueueAllLightDirtyBlocks();
	makeNeighborsDirty();
	}

void Chunk::resetAllLightValuesToZero()
	{
	for(int z=0;z<CHUNK_BLOCKS_Z;z++)
	for(int y=0;y<CHUNK_BLOCKS_Y;y++)
	for(int x=0;x<CHUNK_BLOCKS_X;x++)
		{
		Block* block=getBlockAddrAt(IntVector3(x,y,z));
		if (block)
			{
			block->setLight(0);
			}
		}
	}

void Chunk::initializeLightAndDirtyForSkyBlocks()
	{
	for(int z=0;z<CHUNK_BLOCKS_Z;z++)
	for(int y=0;y<CHUNK_BLOCKS_Y;y++)
	for(int x=0;x<CHUNK_BLOCKS_X;x++)
		{
		BlockInfo bi(this,getBlockIndexForLocalCoords(IntVector3(x,y,z)));
		Block* block=bi.getBlock();
		if (block && block->isSky())
			{				
			block->setLight(SKY_LIGHT);

			bi.makeNeighborhoodLightingDirty();
			}
		}	
	}

void Chunk::initLightAndDirtyForSelfLightedBlocks()
	{
	for(int z=0;z<CHUNK_BLOCKS_Z;z++)
	for(int y=0;y<CHUNK_BLOCKS_Y;y++)
	for(int x=0;x<CHUNK_BLOCKS_X;x++)
		{
		BlockInfo bi(this,getBlockIndexForLocalCoords(IntVector3(x,y,z)));
		Block* block=bi.getBlock();
		if (block)
			{			
			BlockDefinition bdef=block->getDef();
			//int light=block->getLight();
			if(bdef.m_lightEmitted > 0)
				{
				//block->setLight(maxOf((int)bdef.m_lightEmitted,(int)light));
				g_theWorld->enqueueDirtyLightBlock(bi);
				}
			//bi.makeNeighborhoodLightingDirty();
			}
		}	
	}

void Chunk::enqueueAllLightDirtyBlocks()
	{
	for(int z=0;z<CHUNK_BLOCKS_Z;z++)
	for(int y=0;y<CHUNK_BLOCKS_Y;y++)
	for(int x=0;x<CHUNK_BLOCKS_X;x++)
		{
		BlockInfo bi(this,getBlockIndexForLocalCoords(IntVector3(x,y,z)));
		Block* block=bi.getBlock();
		if (block)
			{
			if(block->isLightDirty())
				g_theWorld->enqueueDirtyLightBlock(bi);
			}
		}
	enqueueBorderingLightDirtyBlocks();
	}

void Chunk::enqueueBorderingLightDirtyBlocks()
	{
	if(m_eastNeighbor)
		enqueueEastLightDirtyBlocks();
	if(m_westNeighbor)
		enqueueWestLightDirtyBlocks();
	if(m_southNeighbor)
		enqueueSouthLightDirtyBlocks();
	if(m_northNeighbor)
		enqueueNorthLightDirtyBlocks();
	}

void Chunk::enqueueWestLightDirtyBlocks()
	{
	enqueueLightDirtyBlockRange(IntVector3(0,0,0),IntVector3(0,CHUNK_BLOCKS_Y-1,CHUNK_BLOCKS_Z-1));
	m_isVboDirty=true;
	}

void Chunk::enqueueEastLightDirtyBlocks()
	{
	enqueueLightDirtyBlockRange(IntVector3(CHUNK_BLOCKS_X-1,0,0),IntVector3(CHUNK_BLOCKS_X-1,CHUNK_BLOCKS_Y-1,CHUNK_BLOCKS_Z-1));
	m_isVboDirty=true;
	}

void Chunk::enqueueSouthLightDirtyBlocks()
	{
	enqueueLightDirtyBlockRange(IntVector3(0,0,0),IntVector3(CHUNK_BLOCKS_X-1,0,CHUNK_BLOCKS_Z-1));
	m_isVboDirty=true;
	}

void Chunk::enqueueNorthLightDirtyBlocks()
	{
	enqueueLightDirtyBlockRange(IntVector3(0,CHUNK_BLOCKS_Y-1,0),IntVector3(CHUNK_BLOCKS_X-1,CHUNK_BLOCKS_Y-1,CHUNK_BLOCKS_Z-1));
	m_isVboDirty=true;
	}

void Chunk::makeNeighborsDirty()
	{
	if (m_eastNeighbor)
		m_eastNeighbor->m_isVboDirty=true;
	if (m_westNeighbor)
		m_westNeighbor->m_isVboDirty=true;
	if (m_southNeighbor)
		m_southNeighbor->m_isVboDirty=true;
	if (m_northNeighbor)
		m_northNeighbor->m_isVboDirty=true;
	}

void Chunk::enqueueLightDirtyBlockRange(const IntVector3& mins,const IntVector3& maxs)
	{
	for(int z=mins.z;z<=maxs.z;z++)
	for(int y=mins.y;y<=maxs.y;y++)
	for(int x=mins.x;x<=maxs.x;x++)
		{
		BlockInfo dirtyBlockInfo(this,getBlockIndexForLocalCoords(IntVector3(x,y,z)));
		Block* dirtyBlock=dirtyBlockInfo.getBlock();
		if (dirtyBlock && !dirtyBlock->isOpaque())
			{
			g_theWorld->enqueueDirtyLightBlock(dirtyBlockInfo);
			}
		}
	}

std::vector<Vector3> Chunk::calcCornersInWorldCoords()
	{
	std::vector<Vector3> result;

	AABB3 bounds=calcChunkBoundsInWorld();
	float x,y,z;

	for (int iz=0;iz<=1;iz++)
		{
		z=iz==0?bounds.m_mins.z:bounds.m_maxs.z;
		for (int iy=0;iy<=1;iy++)
			{
			y=iy==0?bounds.m_mins.y:bounds.m_maxs.y;
			for (int ix=0;ix<=1;ix++)
				{
				x=ix==0?bounds.m_mins.x:bounds.m_maxs.x;

				result.push_back(Vector3(x,y,z));
				}
			}
		}

	return result;
	}

std::vector<Vector3> Chunk::calcCornersInCameraCoords(const Vector3& camPos)
	{
	std::vector<Vector3> result;
	result=calcCornersInWorldCoords();
	for (int i=0;i<(int)result.size();i++)
		{
		result.at(i)-=camPos;
		}
	return result;
	}


bool Chunk::isAnyCornerAbovePlane(const Vector3& normal, const Vector3& camPos)
	{
	std::vector<Vector3> chunkCorners=calcCornersInCameraCoords(camPos);

	bool result=false;

	for (Vector3 corner: chunkCorners)
		{
		float dot=corner.dotProduct(normal);
		result=result||dot>0.f;
		}

	return result;
	}

bool Chunk::isAnyCornerWithinRangeOfWorldPoint(const Vector3& camPos, float radius)
	{
	std::vector<Vector3> chunkCorners=calcCornersInCameraCoords(camPos);

	bool result=false;
	float radiusSquared=radius*radius;

	for (Vector3 corner: chunkCorners)
		{
		Vector2 corner2d=Vector2(corner.x,corner.y);
		float distSquared=corner2d.calcLengthSquared();
		result=result||distSquared<radiusSquared;
		}
	while(result)
		break;
	return result;
	}


BLOCK::Type Chunk::calcBiomeSurfaceType(int x,int y,int z,unsigned int seed)
	{
	float fheight=(float)z/(float)CHUNK_BLOCKS_Z;
	float moisture=getBlockMoisture(x,y,seed);
	float temperature=getBaseTemperatureForBlock(x,y);
	BLOCK::Type result=BLOCK::GRASS;
	if(fheight>0.7)
		result=BLOCK::SNOW;
	else if (temperature>0.7f)
		{
		if(moisture>0.7f)
			result=BLOCK::LEAVES;
		else if(moisture>0.3f)
			result=BLOCK::GRASS;
		else
			result=BLOCK::SAND;
		}
	else if (temperature>0.4f)
		{
		if(moisture>0.3f)
			result=BLOCK::GRASS;
		else
			result=BLOCK::DIRT;
		}
	else
		{
		if(moisture>0.2f)
			result=BLOCK::SNOW;
		else
			result=BLOCK::DIRT;
		}
	return result;
	}

void Chunk::updateLightingAfterBlockPlacementAt(const IntVector3& blockCoords)
	{
	//TODO: implement function to update lighting after block placement
	BlockInfo bi(this,getBlockIndexForLocalCoords(blockCoords));
	Block* curBlock=bi.getBlock();

	if(curBlock==nullptr) 
		return;

	curBlock->setSkyBit(false);
	curBlock->setLight(0);
	g_theWorld->enqueueDirtyLightBlock(bi);
	bi.enqueueHorizontalDirtyLightNeighbors();
	bi.enqueueUpDirtyLightNeigbhor();

	curBlock->setSkyBit(false);
	bi.moveDown();
	curBlock=bi.getBlock();
// 
// 	if(!curBlock->isSky())
// 		return;

	while (curBlock && !curBlock->isOpaque())
		{
		curBlock->setSkyBit(false);
		g_theWorld->enqueueDirtyLightBlock(bi);
		bi.enqueueHorizontalDirtyLightNeighbors();
		bi.enqueueUpDirtyLightNeigbhor();

		bi.moveDown();
		curBlock=bi.getBlock();
		}
	}

void Chunk::updateLightingAfterBlockRemovalAt(const IntVector3& blockCoords)
	{
	//TODO: implement function to update lighting after block removal
	BlockInfo bi(this,getBlockIndexForLocalCoords(blockCoords));
	Block* curBlock=bi.getBlock();

	if (curBlock==nullptr)
		return;

	curBlock->setLight(0);
	curBlock->setLightDirtyBit(true);
	g_theWorld->enqueueDirtyLightBlock(bi);

	BlockInfo upInfo=bi.getUp();
	Block* upBlock=upInfo.getBlock();

	if (!(upBlock && upBlock->isSky()))
		return;

	curBlock->setSkyBit(true);
	bi.moveDown();
	curBlock=bi.getBlock();
	while (curBlock && !curBlock->isOpaque())
		{
		curBlock->setLight(0);
		curBlock->setSkyBit(true);
		g_theWorld->enqueueDirtyLightBlock(bi);

		bi.moveDown();
		curBlock=bi.getBlock();
		}
	}

bool Chunk::doesNeighborBlockObstructView(const BlockInfo& blockInfo)
	{
	Block* block=blockInfo.getBlock();
	if(block)
		return block->isSolid();
	return false;
	}

bool Chunk::decodeFileBuffer(std::vector<unsigned char>& buffer,const std::vector<unsigned char>& encodedBuffer)
	{
	if (buffer.size()>0) buffer.clear();
	if(encodedBuffer.size()<4)
		return false;
	int i=0;
	ChunkFileHeader header;
	header.m_version	=	encodedBuffer[i++];
	header.m_bitCountX	=	encodedBuffer[i++];
	header.m_bitCountY	=	encodedBuffer[i++];
	header.m_bitCountZ	=	encodedBuffer[i++];

	if(header.m_version != CURRENT_CHUNK_VERSION 
	   || header.m_bitCountX != CHUNK_X_BITS
	   || header.m_bitCountY != CHUNK_Y_BITS
	   || header.m_bitCountZ != CHUNK_Z_BITS)
		return false;

	int amtBytesProcessed=0;
	while (i+1<(int)encodedBuffer.size())
		{
		unsigned char data=encodedBuffer[i++];
		unsigned char repeatCount=encodedBuffer[i++];
		for (int counter=0;counter<repeatCount;counter++)
			{
			buffer.push_back(data);
			amtBytesProcessed++;
			}
		}

	if(amtBytesProcessed<BLOCKS_PER_CHUNK)
		return false;

	return true;
	}

bool Chunk::populateFromRawData(const std::vector<unsigned char>& buffer)
	{
	int i=0;
	for (int z=0;z<CHUNK_BLOCKS_Z;z++)
	for (int y=0;y<CHUNK_BLOCKS_Y;y++)
	for (int x=0;x<CHUNK_BLOCKS_X;x++)
		{
		Block* b=getBlockAddrAt(IntVector3(x,y,z));
		if(b==nullptr)
			return false;
		if(i<(int)buffer.size())
			b->setType((BLOCK::Type)buffer.at(i++));
		else
			return false;
		}	
	return true;
	}

std::string Chunk::buildMyFileName()
	{
	std::stringstream ss;
	ss.str("");
	//Data/SaveSlot0/Chunk_at_(22,-15).chunk
	ss<<"Data/SaveSlot0/Chunk_at_(";
	ss<<m_chunkCoords.x;
	ss<<",";
	ss<<m_chunkCoords.y;
	ss<<").chunk";
	return ss.str();
	}


IntVector2 Chunk::convertBlockCoordsToWorldCoordsInt(const IntVector2& pos)
	{
	int wx=pos.x+m_chunkCoords.x*CHUNK_BLOCKS_X;
	int wy=pos.y+m_chunkCoords.y*CHUNK_BLOCKS_Y;
	return IntVector2(wx,wy);
	}

Vector3 Chunk::calcBlockCenter(const IntVector3& blockCoords)
	{
	Vector3 result=convertBlockCoordsToWorldCoords3(blockCoords);
	float halfsize=0.5f*BLOCK_CUBE_SIZE;
	result+=Vector3(halfsize,halfsize,halfsize);
	return result;
	}

void Chunk::populateFromSineWave()
	{
	//TODO: populate all blocks in chunk from deterministic noise function.
	for (int iy=0;iy<CHUNK_BLOCKS_Y;iy++)
		for (int ix=0;ix<CHUNK_BLOCKS_X;ix++)
			{
			int x = ix + this->m_chunkCoords.x;
			int y = iy + this->m_chunkCoords.y;
			int z = calcSineHeight(x, y);
			addBlockStackAt(IntVector3(ix,iy,z));

			// TODO: activate the following line after getting the noise to work.
			// int z = getNoiseHeightAt(x,y);

			}

	}


