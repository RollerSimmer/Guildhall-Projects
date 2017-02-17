#pragma once

const float MAX_STAR_SIZE = 7.5f; 
const float MAX_STAR_RADIUS = 0.5f*MAX_STAR_SIZE;

#include "Engine/Math/Disc2.hpp"
#include "Engine/Renderer/Rgba.hpp"

class Star
	{
	private:
		Disc2 m_geometry;
		Rgba m_color;
	public:
		Star();
		void render();
		void renderStarLine(int frame);
	};

