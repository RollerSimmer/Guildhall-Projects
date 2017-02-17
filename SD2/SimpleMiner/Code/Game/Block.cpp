#include "Game/Block.hpp"
#include <assert.h>
#include "Game/GameCommon.hpp"

BlockDefinition		BlockDefinition::s_definitions[BLOCK::AMT_TYPES];

BlockDefinition::BlockDefinition(BLOCK::Type settype,const AABB2& topTexCoords,
								 const AABB2& bottomTexCoords,const AABB2& sideTexCoords,
								 bool isVisible,bool isSolid)
	: m_type(settype),
	m_topTexCoords(topTexCoords),
	m_bottomTexCoords(bottomTexCoords),
	m_sideTexCoords(sideTexCoords),
	m_isOpaque(isVisible),
	m_isSolid(isSolid),
	m_walkSound(0),
	m_nameString("")
	{

	}

BlockDefinition::BlockDefinition()
	: m_type(BLOCK::AIR)
	, m_isOpaque(false)
	{

	}

void BlockDefinition::initBlockDefinitions()
	{
	assert(g_tileAtlas!=nullptr);
	BlockDefinition* curdef;

	//air 
	BlockDefinition& air=s_definitions[BLOCK::AIR];
	curdef=&air;
	curdef->m_type=BLOCK::AIR;
	curdef->m_topTexCoords=g_tileAtlas->getTexCoordsForSpriteCoords(15,15);
	curdef->m_bottomTexCoords=air.m_topTexCoords;
	curdef->m_sideTexCoords=air.m_bottomTexCoords;
	curdef->m_isSolid=false;
	curdef->m_isOpaque=false;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("AIR");

	//grass
	BlockDefinition& grass=s_definitions[BLOCK::GRASS];
	curdef=&grass;
	curdef->m_type=BLOCK::GRASS;
	curdef->m_topTexCoords=g_tileAtlas->getTexCoordsForSpriteCoords(9,8);
	curdef->m_bottomTexCoords=g_tileAtlas->getTexCoordsForSpriteCoords(7,8);
	curdef->m_sideTexCoords=g_tileAtlas->getTexCoordsForSpriteCoords(8,8);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("GRASS");

	//dirt
	BlockDefinition& dirt=s_definitions[BLOCK::DIRT];
	curdef=&dirt;
	curdef->m_type=BLOCK::DIRT;
	curdef->m_topTexCoords=g_tileAtlas->getTexCoordsForSpriteCoords(7,8);
	curdef->m_bottomTexCoords=g_tileAtlas->getTexCoordsForSpriteCoords(7,8);
	curdef->m_sideTexCoords=g_tileAtlas->getTexCoordsForSpriteCoords(7,8);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("DIRT");

	//stone
	BlockDefinition& stone=s_definitions[BLOCK::STONE];
	curdef=&stone;
	curdef->m_type=BLOCK::STONE;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(2,10);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(2,10);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(2,10);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("STONE");

	//water
	BlockDefinition& water=s_definitions[BLOCK::WATER];
	curdef=&water;
	curdef->m_type=BLOCK::WATER;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(8,11);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(8,11);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(8,11);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;	
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("WATER");

	//gray brick
	BlockDefinition& graybrick=s_definitions[BLOCK::GRAY_BRICK];
	curdef=&graybrick;
	curdef->m_type=BLOCK::GRAY_BRICK;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(12,11);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(12,11);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(12,11);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("BRICK");

	//sand
	BlockDefinition& sand=s_definitions[BLOCK::SAND];
	curdef=&sand;
	curdef->m_type=BLOCK::SAND;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(1,8);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(1,8);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(1,8);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("SAND");

	//gravel
	BlockDefinition& gravel=s_definitions[BLOCK::GRAVEL];
	curdef=&gravel;
	curdef->m_type=BLOCK::GRAVEL;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(0,10);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(0,10);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(0,10);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("GRAVEL");

	//glowstone
	BlockDefinition& glowstone=s_definitions[BLOCK::GLOWSTONE];
	curdef=&glowstone;
	curdef->m_type=BLOCK::GLOWSTONE;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(4,11);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(4,11);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(4,11);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=12;
	curdef->m_nameString=std::string("GLOWSTONE");

	//wood
	BlockDefinition& wood=s_definitions[BLOCK::WOOD];
	curdef=&wood;
	curdef->m_type=BLOCK::WOOD;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(13,8);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(13,8);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(13,8);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("WOOD");

	//tree
	BlockDefinition& tree=s_definitions[BLOCK::TREE];
	curdef=&tree;
	curdef->m_type=BLOCK::TREE;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(14,10);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(14,10);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(14,10);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("TREE");

	//leaves
	BlockDefinition& leaves=s_definitions[BLOCK::LEAVES];
	curdef=&leaves;
	curdef->m_type=BLOCK::LEAVES;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(12,8);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(12,8);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(12,8);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("LEAVES");

	//snow
	BlockDefinition& snow=s_definitions[BLOCK::SNOW];
	curdef=&snow;
	curdef->m_type=BLOCK::SNOW;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(0,8);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(1,9);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(1,9);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("SNOW");

	//sandstone
	BlockDefinition& sandstone=s_definitions[BLOCK::SANDSTONE];
	curdef=&sandstone;
	curdef->m_type=BLOCK::SANDSTONE;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(6,11);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(6,11);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(6,11);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("SANDSTONE");

	//brick
	BlockDefinition& brick=s_definitions[BLOCK::BRICK];
	curdef=&brick;
	curdef->m_type=BLOCK::BRICK;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(3,11);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(3,11);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(3,11);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("BRICK");

	//masonry
	BlockDefinition& masonry=s_definitions[BLOCK::MASONRY];
	curdef=&masonry;
	curdef->m_type=BLOCK::MASONRY;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(11,10);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(11,10);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(12,10);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("MASONRY");

	//glass
	BlockDefinition& glass=s_definitions[BLOCK::GLASS];
	curdef=&glass;
	curdef->m_type=BLOCK::GLASS;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords(7,11);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords(7,11);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords(7,11);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("GLASS");

	//asphalt
	BlockDefinition& asphalt=s_definitions[BLOCK::ASPHALT];
	curdef=&asphalt;
	curdef->m_type=BLOCK::ASPHALT;
	curdef->m_topTexCoords    = g_tileAtlas->getTexCoordsForSpriteCoords( 1,10);
	curdef->m_bottomTexCoords = g_tileAtlas->getTexCoordsForSpriteCoords( 1,10);
	curdef->m_sideTexCoords   = g_tileAtlas->getTexCoordsForSpriteCoords( 1,10);
	curdef->m_isSolid=true;
	curdef->m_isOpaque=true;
	curdef->m_lightEmitted=0;
	curdef->m_nameString=std::string("ASPHALT");
	}

void BlockDefinition::initBlockWalkSounds()
	{
	if(g_theAudioSys==nullptr)
		return;

	SoundID rockWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/rockWalk.ogg"));
// 	SoundID carpetWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/carpetWalk.ogg"));
	SoundID grassWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/grassWalk.ogg"));
// 	SoundID rubberWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/rubberWalk.ogg"));
	SoundID dirtWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/dirtWalk.ogg"));
	SoundID leafWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/leafWalk.ogg"));
// 	SoundID tileWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/tileWalk.ogg"));
	SoundID woodWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/woodWalk.ogg"));
// 	SoundID metalWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/metalWalk.ogg"));
	SoundID waterWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/waterWalk.ogg"));
	SoundID gravelWalkSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/gravelWalk.ogg"));

	//assign every block sound to default sound ID
	for(int i=0;i<BLOCK::AMT_TYPES;i++)
		s_definitions[i].m_walkSound=rockWalkSound;

	s_definitions[BLOCK::GRASS]		.m_walkSound=grassWalkSound;
	s_definitions[BLOCK::DIRT]		.m_walkSound=dirtWalkSound;
	s_definitions[BLOCK::WATER]		.m_walkSound=waterWalkSound;
	s_definitions[BLOCK::SAND]		.m_walkSound=dirtWalkSound;
	s_definitions[BLOCK::GRAVEL]	.m_walkSound=gravelWalkSound;
	s_definitions[BLOCK::LEAVES]	.m_walkSound=leafWalkSound;
	s_definitions[BLOCK::WOOD]		.m_walkSound=woodWalkSound;
	s_definitions[BLOCK::TREE]		.m_walkSound=woodWalkSound;
	}

void BlockDefinition::initBlockPlacementSounds()
	{
	if (g_theAudioSys==nullptr)
		return;

	SoundID rockPlacementSound			=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/rockPlacement.ogg"));
	// 	SoundID carpetPlacementSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/carpetPlacement.ogg"));
	// 	SoundID rubberPlacementSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/rubberPlacement.ogg"));
	SoundID dirtPlacementSound			=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/dirtPlacement.ogg"));
	SoundID leafPlacementSound			=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/leafPlacement.ogg"));
	// 	SoundID tilePlacementSound		=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/tilePlacement.ogg"));
	SoundID woodPlacementSound			=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/woodPlacement.ogg"));
	// 	SoundID metalPlacementSound		=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/metalPlacement.ogg"));
	SoundID waterPlacementSound			=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/placement/waterPlacement.ogg"));

	//assign every block sound to default sound ID
	for (int i=0;i<BLOCK::AMT_TYPES;i++)
		s_definitions[i].m_placementSound=rockPlacementSound;

	s_definitions[BLOCK::GRASS]		.m_placementSound=dirtPlacementSound;
	s_definitions[BLOCK::DIRT]		.m_placementSound=dirtPlacementSound;
	s_definitions[BLOCK::WATER]		.m_placementSound=waterPlacementSound;
	s_definitions[BLOCK::SAND]		.m_placementSound=dirtPlacementSound;
	s_definitions[BLOCK::GRAVEL]	.m_placementSound=rockPlacementSound;
	s_definitions[BLOCK::LEAVES]	.m_placementSound=leafPlacementSound;
	s_definitions[BLOCK::WOOD]		.m_placementSound=woodPlacementSound;
	s_definitions[BLOCK::TREE]		.m_placementSound=woodPlacementSound;
	}

void BlockDefinition::initBlockRemovalSounds()
	{
	if (g_theAudioSys==nullptr)
		return;

	SoundID rockRemovalSound		=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/rockRemoval.ogg"));
	// 	SoundID carpetRemovalSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/carpetRemoval.ogg"));
	// 	SoundID rubberRemovalSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/rubberRemoval.ogg"));
	SoundID dirtRemovalSound		=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/dirtRemoval.ogg"));
	SoundID leafRemovalSound		=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/leafRemoval.ogg"));
	// 	SoundID tileRemovalSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/tileRemoval.ogg"));
	SoundID woodRemovalSound		=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/woodRemoval.ogg"));
	// 	SoundID metalRemovalSound	=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/metalRemoval.ogg"));
	SoundID waterRemovalSound		=	g_theAudioSys->createOrGetSound(std::string("Data/Audio/removal/waterRemoval.ogg"));

	//assign every block sound to default sound ID
	for (int i=0;i<BLOCK::AMT_TYPES;i++)
		s_definitions[i].m_removalSound=rockRemovalSound;

	s_definitions[BLOCK::GRASS]		.m_removalSound=dirtRemovalSound;
	s_definitions[BLOCK::DIRT]		.m_removalSound=dirtRemovalSound;
	s_definitions[BLOCK::WATER]		.m_removalSound=waterRemovalSound;
	s_definitions[BLOCK::SAND]		.m_removalSound=dirtRemovalSound;
	s_definitions[BLOCK::GRAVEL]	.m_removalSound=rockRemovalSound;
	s_definitions[BLOCK::LEAVES]	.m_removalSound=leafRemovalSound;
	s_definitions[BLOCK::WOOD]		.m_removalSound=woodRemovalSound;
	s_definitions[BLOCK::TREE]		.m_removalSound=woodRemovalSound;
	}

int BlockDefinition::clampBlockIndex(int requestedBlockIndex)
	{
	if(requestedBlockIndex<0)
		return 0;
	if(requestedBlockIndex>=BLOCK::AMT_TYPES)
		return BLOCK::AMT_TYPES-1;
	return requestedBlockIndex;
	}

Block::Block()
	{
	m_type=BLOCK::AIR;
	BlockDefinition airdef=BlockDefinition::s_definitions[BLOCK::AIR];
	setSkyBit(false);
	setOpaqueBit(airdef.m_isOpaque);
	setSolidBit(airdef.m_isSolid);
	setLightDirtyBit(false);
	setLight(0);
	m_flagsAndLight=0;
/*	setLight(GetRandomIntInRange(8,15));*/
	}

BlockDefinition Block::getDef() const
	{
	BlockDefinition result;
	result=BlockDefinition::s_definitions[m_type];
	return result;
	}

void Block::setType(unsigned char btype)
	{
	m_type=btype;
	BlockDefinition bdef=getDef();
	setOpaqueBit(bdef.m_isOpaque ? 1 : 0);
	setSolidBit(bdef.m_isSolid ? 1 : 0);
	setLightDirtyBit(false);
	setLight(0);
	}
