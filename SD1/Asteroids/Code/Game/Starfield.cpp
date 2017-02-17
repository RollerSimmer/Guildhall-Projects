#include "Starfield.hpp"
#include "Game/Starfield.hpp"

Starfield::Starfield()
	: m_stars{}
	{
	init();
	}

void Starfield::render() const
	{
	for (int i = 0; i < AMT_STARS_IN_STARFIELD; i++)
		{
		if (m_stars[i]!=nullptr)
			m_stars[i]->render();		
		}
	}

void Starfield::init()
	{
	for (int i = 0; i < AMT_STARS_IN_STARFIELD; i++)
		{
		m_stars[i]=new Star();
		}
	}

void Starfield::renderStarLines(int frame)
	{
	for (int i = 0; i < AMT_STARS_IN_STARFIELD; i++)
		{
		if (m_stars[i] != nullptr)
			m_stars[i]->renderStarLine(frame);
		}
	}
