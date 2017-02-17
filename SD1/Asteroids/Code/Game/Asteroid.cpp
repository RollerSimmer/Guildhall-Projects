#include "Game/Asteroid.hpp"
#include <cmath>
#include "Game/GameCommon.hpp"

Asteroid::Asteroid(const Vector2& pos)
	: Entity(pos, ASTEROID_RADIUS_LARGE, FIXED_ASTEROID_SPEED)
	{
	initVertexOffsets();
	m_mass = ASTEROID_MASS_LARGE;
	}

void Asteroid::render() const
{
	g_theRenderer->drawOffsetRotatedRegularPolygon(m_pos.x,m_pos.y,
		AMT_ASTEROID_SIDES,m_physicsRadius,m_orientationDegrees,m_vertexOffsets);
	Entity::render();
	}

void Asteroid::initVertexOffsets()
	{
	float scale=this->m_physicsRadius*0.23f;
	for (int i = 0; i < AMT_ASTEROID_VERTICES; i++)
		{
		m_vertexOffsets[i].x = GetRandomFloatInRange(-scale,scale);
		m_vertexOffsets[i].y = GetRandomFloatInRange(-scale,scale);
		}
	}

void Asteroid::shrinkAsteroidForSplit()
	{
	setRadii(m_graphicsRadius*ASTEROID_SPLIT_SHRINK_RADIUS_SCALE);
	m_mass *= ASTEROID_SPLIT_SHRINK_MASS_SCALE;
	}

bool Asteroid::hasReachedSmallestSize()
	{
	return m_mass <= ASTEROID_MASS_SMALL;
	}

void Asteroid::update(float deltaSeconds)
	{
	Entity::update(deltaSeconds);
	}


