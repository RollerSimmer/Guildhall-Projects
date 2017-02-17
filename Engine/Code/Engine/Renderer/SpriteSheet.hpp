#pragma once

#include <string>
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Rgba.hpp"

class SpriteSheet
	{
	public:
		SpriteSheet(Renderer* theRenderer,const std::string& imageFilePath, int tilesWide, int tilesHigh);
		SpriteSheet(Renderer* theRenderer, Texture& texture, int tilesWide, int tilesHigh);
		AABB2 getTexCoordsForSpriteCoords(int spriteX, int spriteY) const; // mostly for atlases
		AABB2 getTexCoordsForSpriteIndex(int spriteIndex) const; // mostly for sprite animations
		int getNumSprites() const;
		//... and other member functions as needed
		Texture* getTexture() const { return m_spriteSheetTexture; }
		std::string getTextureFilePath();
		void drawSprite(const AABB2& bounds, int index, const Rgba& tint=Rgba::WHITE, float orientationDegrees=0.f);
		void drawSpriteByPositionRadius(const Vector2& pos,float radius,int index, const Rgba& tint=Rgba::WHITE, float orientationDegrees=0.f);
	private:
		Texture* 	m_spriteSheetTexture;	// Texture with grid-based layout of sub-textures
		IntVector2	m_spriteLayout;		// # of sprites across, and down, on the sheet
										//... and other data member variables as needed
		Vector2     m_invSpriteLayout;
		Renderer*   m_theRenderer;
		std::string m_filepath;
	};
