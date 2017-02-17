#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Rgba.hpp"

enum TileType
	{
	TILE_TYPE_GRASS=0,
	TILE_TYPE_STONE,
	TILE_TYPE_START_PLATFORM,
	TILE_TYPE_END_PLATFORM,
	AMT_TILE_TYPES
	};

struct TileDef
	{
	static bool s_defInit; // = false;
	TileType m_type;
	bool m_canPassThru;
	bool m_canSeeThru;
	bool m_isEndTrigger;
	Rgba m_color;
	TileDef(TileType ttype=TILE_TYPE_GRASS,bool canPassThru=true, bool canSeeThru=true, bool isEndTrigger=false,const Rgba& color=Rgba::GREEN);
	};

class Tile
	{
	public:
		Tile(TileType tiletype=TILE_TYPE_GRASS,const IntVector2& pos=IntVector2::ZERO);
		~Tile();
		IntVector2 getpos() const { return m_pos; }
		TileDef getdef() const;
	private:
		TileType m_type;
		IntVector2 m_pos;
	};

extern TileDef g_tileDefs[AMT_TILE_TYPES];