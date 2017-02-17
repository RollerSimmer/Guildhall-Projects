#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Audio/Audio.hpp"

constexpr unsigned char BLOCK_MASK_LIGHT       = 0b00001111; // (bits 0-3) are light value, 0 to 15
constexpr unsigned char BLOCK_MASK_IS_SKY      = 0b10000000; // (bit 7) is on if block sees sky
constexpr unsigned char BLOCK_MASK_LIGHT_DIRTY = 0b01000000; // (bit 6) is on if light is dirty
constexpr unsigned char BLOCK_MASK_IS_OPAQUE   = 0b00100000; // (bit 5) on if block type is opaque
constexpr unsigned char BLOCK_MASK_IS_SOLID	   = 0b00010000; // (bit 4) on if block type is solid

namespace BLOCK
	{
	enum Type
		{
		GRASS=0,
		DIRT,
		STONE,
		WATER,
		GRAY_BRICK,
		SAND,
		GLOWSTONE,
		GRAVEL,
		WOOD,
		TREE,
		LEAVES,
		SNOW,
		SANDSTONE,
		BRICK,
		MASONRY,
		GLASS,
		ASPHALT,
		AIR,
		AMT_TYPES
		};
	}

struct BlockDefinition
	{
	BlockDefinition();
	BlockDefinition(BLOCK::Type settype,const AABB2& topTexCoords,
					const AABB2& bottomTexCoords,const AABB2& sideTexCoords,
					bool isVisible,bool isSolid);
	static void initBlockDefinitions();
	static void initBlockWalkSounds();
	static void initBlockPlacementSounds();
	static void initBlockRemovalSounds();
	static int clampBlockIndex(int requestedBlockIndex);

	static BlockDefinition s_definitions[BLOCK::AMT_TYPES];

	BLOCK::Type m_type;
	AABB2 m_topTexCoords;
	AABB2 m_bottomTexCoords;
	AABB2 m_sideTexCoords;
	bool m_isOpaque;
	bool m_isSolid;
	unsigned char m_lightEmitted;
	std::string m_nameString;
	SoundID m_walkSound;
	SoundID m_placementSound;
	SoundID m_removalSound;
	};

const int SKY_LIGHT=6;

struct Block
	{
	unsigned char m_type;
	unsigned char m_flagsAndLight;

	Block();
	
	BlockDefinition getDef() const;
	void setType(unsigned char btype);

	bool isSky() const;
	bool isSolid() const;
	bool isOpaque() const;
	bool isLightDirty() const;
	int getLight() const;

	void setSkyBit(bool skyBit);
	void setLightDirtyBit(bool dirtybit);
	void setOpaqueBit(bool opaqueBit);
	void setSolidBit(bool solidBit);
	void setLight(int light);
	};

//----------------------------------------------------------------------

inline bool Block::isSky() const
	{
	return (m_flagsAndLight&BLOCK_MASK_IS_SKY)==BLOCK_MASK_IS_SKY;
	}

inline void Block::setSkyBit(bool skyBit)
	{
	m_flagsAndLight &= ~BLOCK_MASK_IS_SKY;
	if (skyBit)
		m_flagsAndLight |= BLOCK_MASK_IS_SKY;
	}

inline bool Block::isSolid() const
	{
	return (m_flagsAndLight&BLOCK_MASK_IS_SOLID)==BLOCK_MASK_IS_SOLID;
	}

inline bool Block::isOpaque() const
	{
	return (m_flagsAndLight&BLOCK_MASK_IS_OPAQUE)==BLOCK_MASK_IS_OPAQUE;
	}

inline bool Block::isLightDirty() const
	{
	return (m_flagsAndLight&BLOCK_MASK_LIGHT_DIRTY)==BLOCK_MASK_LIGHT_DIRTY;
	}

inline void Block::setLightDirtyBit(bool dirtybit)
	{
	m_flagsAndLight &= ~BLOCK_MASK_LIGHT_DIRTY;
	if(dirtybit)
		m_flagsAndLight |= BLOCK_MASK_LIGHT_DIRTY; 
	}

inline void Block::setOpaqueBit(bool opaqueBit)
	{
	m_flagsAndLight &= ~BLOCK_MASK_IS_OPAQUE;
	if (opaqueBit)
		m_flagsAndLight |= BLOCK_MASK_IS_OPAQUE;
	}

inline void Block::setSolidBit(bool solidBit)
	{
	m_flagsAndLight &= ~BLOCK_MASK_IS_SOLID;
	if (solidBit)
		m_flagsAndLight |= BLOCK_MASK_IS_SOLID;
	}


inline int Block::getLight() const
	{
	return m_flagsAndLight&BLOCK_MASK_LIGHT;
	}

inline void Block::setLight(int light)
	{
	light&=BLOCK_MASK_LIGHT;
	m_flagsAndLight&=~BLOCK_MASK_LIGHT;
	m_flagsAndLight|=light;
	}
