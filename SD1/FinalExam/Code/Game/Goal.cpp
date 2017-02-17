#include "Game/Goal.hpp"
#include "Game/GameCommon.hpp"

Goal::Goal()
	: Entity(Vector2(1450.f,750.f),ENTITY_TYPE_GOAL,true,10.f,0.f,true)
	{
	m_cosmeticRadius=50.f;
	m_movingSpriteTexture=g_theRenderer->CreateOrGetTexture("Data/Images/Goal.png");

	m_spinRateDegrees=180.f;
	m_isSpinning=true;
	}
