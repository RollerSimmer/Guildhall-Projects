#include "Engine/Renderer/SpriteSheet.hpp"

SpriteSheet::SpriteSheet(Renderer* theRenderer, const std::string& imageFilePath, int tilesWide, int tilesHigh)
	: m_spriteLayout(tilesWide,tilesHigh)
	, m_invSpriteLayout(1.f / (float)tilesWide, 1.f / (float)tilesHigh)
	, m_theRenderer(theRenderer)
	{
	m_theRenderer=theRenderer;
	if(m_theRenderer)													   
		m_spriteSheetTexture=m_theRenderer->CreateOrGetTexture(imageFilePath);
	}

SpriteSheet::SpriteSheet(Renderer* theRenderer, Texture& texture, int tilesWide, int tilesHigh)
	: m_spriteLayout(tilesWide, tilesHigh)
	, m_invSpriteLayout(1.f/(float)tilesWide,1.f/(float)tilesHigh)
	, m_theRenderer(theRenderer) 
	, m_spriteSheetTexture(&texture)
	{
	
	}

AABB2 SpriteSheet::getTexCoordsForSpriteCoords(int spriteX, int spriteY) const
	{
	Vector2 mins(m_invSpriteLayout.x*(float)spriteX,m_invSpriteLayout.y*(float)spriteY);
	Vector2 maxs(mins.x+m_invSpriteLayout.x, mins.y+m_invSpriteLayout.y);
	return AABB2(mins,maxs);
	}

AABB2 SpriteSheet::getTexCoordsForSpriteIndex(int spriteIndex) const
	{
	int spriteY=spriteIndex/m_spriteLayout.x;
	int spriteX=spriteIndex-(spriteY*m_spriteLayout.x);
	return getTexCoordsForSpriteCoords(spriteX,spriteY);
	}

int SpriteSheet::getNumSprites() const
	{
	return m_spriteLayout.x*m_spriteLayout.y;
	}

std::string SpriteSheet::getTextureFilePath()
	{
	if (m_spriteSheetTexture)
		return m_spriteSheetTexture->m_imageFilePath;
	else
		return std::string("");
	}

void SpriteSheet::drawSprite(const AABB2& bounds, int index, const Rgba& tint/*=Rgba::WHITE*/, float orientationDegrees/*=0.f*/)
	{
	if (m_theRenderer && m_spriteSheetTexture)
		{
		AABB2 texcoords=getTexCoordsForSpriteIndex(index);
		m_theRenderer->drawTexturedAABB2(bounds,*m_spriteSheetTexture,texcoords,tint,orientationDegrees);
		}
	}

void SpriteSheet::drawSpriteByPositionRadius(const Vector2& pos,float radius,int index,const Rgba& tint/*=Rgba::WHITE*/,float orientationDegrees/*=0.f*/)
	{
	Vector2 radiusDisplacement(radius,radius);
	drawSprite(AABB2(pos-radiusDisplacement,pos+radiusDisplacement),index,tint,orientationDegrees);
	}
