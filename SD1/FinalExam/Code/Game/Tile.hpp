#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/Texture.hpp"

enum TileType
	{
	TILE_TYPE_WALL=0,
	TILE_TYPE_SAND,
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
	Texture* m_texture;
	TileDef(TileType ttype=TILE_TYPE_WALL,bool canPassThru=true, bool canSeeThru=true, bool isEndTrigger=false,const Rgba& color=Rgba::WHITE, Texture* texture=nullptr);
	};

class Tile
	{
	public:
		bool m_isOffLimits;
		Tile(TileType tiletype=TILE_TYPE_WALL,const IntVector2& pos=IntVector2::ZERO);
		~Tile();
		IntVector2 getpos() const { return m_pos; }
		TileDef getdef() const;
		Texture* getTexture() const;

		void settype(TileType newtype);
		void setpos(const IntVector2& newpos);
	private:
		TileType m_type;
		IntVector2 m_pos;
	};

extern TileDef g_tileDefs[AMT_TILE_TYPES];