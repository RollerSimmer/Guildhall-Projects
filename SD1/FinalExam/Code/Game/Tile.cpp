#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"

TileDef g_tileDefs[AMT_TILE_TYPES];

bool TileDef::s_defInit = false;

TileDef::TileDef(TileType ttype/*=TILE_TYPE_WALL*/,bool canPassThru/*=true*/, bool canSeeThru/*=true*/, 
				 bool isEndTrigger/*=false*/,const Rgba& color/*=Rgba::WHITE*/, Texture* texture/*=nullptr*/) 
	: m_type(ttype)
	, m_canPassThru(canPassThru)
	, m_canSeeThru(canSeeThru)
	, m_isEndTrigger(isEndTrigger)
	, m_color(color)
	, m_texture(texture)
	{

	}

Tile::Tile(TileType tiletype,const IntVector2& pos)
	: m_type(tiletype)
	, m_pos(pos)
	{
	if (!TileDef::s_defInit)
		{
		g_tileDefs[TILE_TYPE_WALL] = TileDef(TILE_TYPE_WALL, false, false, false, Rgba::WHITE,
											 g_theRenderer->CreateOrGetTexture("Data/Images/Wall.png"));
		g_tileDefs[TILE_TYPE_SAND] = TileDef(TILE_TYPE_SAND, true, true, false, Rgba::WHITE,
											 g_theRenderer->CreateOrGetTexture("Data/Images/sand.png"));
		TileDef::s_defInit=true;
		}
	}


Tile::~Tile()
	{
	}

TileDef Tile::getdef() const
	{
	return g_tileDefs[m_type];
	}

Texture* Tile::getTexture() const
	{
	TileDef tdef = getdef();
	Texture* result=tdef.m_texture;
	return result;
	}

void Tile::settype(TileType newtype)
	{
	m_type=newtype;
	}

void Tile::setpos(const IntVector2& newpos)
	{
	m_pos=newpos;
	}
