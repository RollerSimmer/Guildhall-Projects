#include "Game/Tile.hpp"

TileDef g_tileDefs[AMT_TILE_TYPES];

bool TileDef::s_defInit = false;

TileDef::TileDef(TileType ttype/*=TILE_TYPE_GRASS*/,bool canPassThru/*=true*/, bool canSeeThru/*=true*/, bool isEndTrigger/*=false*/,const Rgba& color/*=Rgba::GREEN*/) : m_type(ttype)
	, m_canPassThru(canPassThru)
	, m_canSeeThru(canSeeThru)
	, m_isEndTrigger(isEndTrigger)
	, m_color(color)
	{

	}

Tile::Tile(TileType tiletype,const IntVector2& pos)
	: m_type(tiletype)
	, m_pos(pos)
	{
	if (!TileDef::s_defInit)
		{
		g_tileDefs[TILE_TYPE_GRASS] = TileDef(TILE_TYPE_GRASS, true, true, false, Rgba::GREEN);
		g_tileDefs[TILE_TYPE_STONE] = TileDef(TILE_TYPE_STONE, false, false, false, Rgba::GRAY);
		g_tileDefs[TILE_TYPE_START_PLATFORM] = TileDef(TILE_TYPE_START_PLATFORM,true,true, false,Rgba::WHITE);
		g_tileDefs[TILE_TYPE_END_PLATFORM] = TileDef(TILE_TYPE_END_PLATFORM,true,true,true,Rgba::WHITE);
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
