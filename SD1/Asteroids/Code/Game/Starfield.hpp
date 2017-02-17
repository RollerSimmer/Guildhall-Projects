#pragma once

#include "Game/Star.hpp"

const int AMT_STARS_IN_STARFIELD = 300;

class Starfield
	{
	private:
		Star* m_stars[AMT_STARS_IN_STARFIELD];
	public:
		Starfield();
		void render() const;
		void init();
		void renderStarLines(int frame);
	};

