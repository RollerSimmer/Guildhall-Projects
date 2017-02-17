#include "Engine/Renderer/BitmapFont.hpp"

BitmapFont::BitmapFont(Renderer* theRenderer, const std::string& imageFilePath, int tilesWide, int tilesHigh)
	: SpriteSheet(theRenderer, imageFilePath, tilesWide, tilesHigh)
	{

	}

AABB2 BitmapFont::getTexCoordsForChar(char character) const
	{
	int spriteIndex=(int)character;
	return getTexCoordsForSpriteIndex(spriteIndex);
	}
