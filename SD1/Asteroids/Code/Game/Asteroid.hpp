#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"

const float ASTEROID_RADIUS_LARGE = 0.1f*g_worldHeight;
const float ASTEROID_MASS_LARGE = 150.f;
const float ASTEROID_MASS_SMALL = ASTEROID_MASS_LARGE*0.5f*0.5f;
const float FIXED_ASTEROID_SPEED = 150.f;
const int AMT_ASTEROID_SIDES = 12;
const int AMT_ASTEROID_VERTICES = 12;
const float ASTEROID_ELASTICITY = 0.95f;
const float ASTEROID_SPLIT_SHRINK_MASS_SCALE = 0.5f;
const float ASTEROID_SPLIT_SHRINK_RADIUS_SCALE = 0.7071F;


class Asteroid: public Entity
{
public:
	const bool SHOULD_DRAW_DEBUG_GRAPHICS = true; 
public:
	Asteroid(const Vector2& pos);
protected:
private:
	Vector2 m_vertexOffsets[AMT_ASTEROID_VERTICES];
// public: // accessors
public:
	virtual void update(float deltaSeconds);
	virtual void render() const;
	void initVertexOffsets();
	void shrinkAsteroidForSplit();
	bool hasReachedSmallestSize();
	};