#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"

class BitmapFont: public SpriteSheet
	{
	public:
		BitmapFont(Renderer* theRenderer, const std::string& imageFilePath, int tilesWide, int tilesHigh);
		AABB2 getTexCoordsForChar(char character) const;
	};

