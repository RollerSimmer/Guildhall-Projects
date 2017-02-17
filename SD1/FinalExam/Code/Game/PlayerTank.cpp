#include "PlayerTank.hpp"
#include "GameCommon.hpp"
#include "Engine\Math\MathUtil.hpp"

PlayerTank::PlayerTank(const Vector2& pos,float radius,float speed,int hpCapacity)
	: Entity(pos,ENTITY_TYPE_PLAYER_TANK,false,radius,speed,hpCapacity,false)
	{
	m_movingSpriteTexture=g_theRenderer->CreateOrGetTexture("Data/Images/Buggy.png");
	}

void PlayerTank::render() const
	{
	Entity::render();
	}

void PlayerTank::update(float deltaSeconds)
	{
	updateInput(deltaSeconds);
	Entity::update(deltaSeconds);
	}

void PlayerTank::updateInput(float /*deltaSeconds*/)
	{
	XboxController& xbc=g_theInput->getXboxController(XCONTROLLER_0);
	float lstickX = xbc.getStickValue(XSTICK_LEFT,X_AXIS_2D);
	float lstickY = xbc.getStickValue(XSTICK_LEFT,Y_AXIS_2D);
	float xdir = 0.f;
	float ydir = 0.f;
	xdir=lstickX;
	ydir=lstickY;
	if (g_theInput->isKeyDown('W'))
		ydir = 1.f;
	if (g_theInput->isKeyDown('S'))
		ydir = -1.f;
	if (g_theInput->isKeyDown('A'))
		xdir = -1.f;
	if (g_theInput->isKeyDown('D'))
		xdir = 1.f;
	if (xdir != 0.f || ydir != 0.f)
		{
		m_vel=Vector2(xdir,ydir);
		m_orientationDegrees=atan2Degrees(ydir,xdir);
		float speedFactor=minOf(m_vel.CalcLength(),1.f);
		m_vel*=PLAYER_TANK::SPEED*speedFactor;
		}
	else
		{
		m_vel=Vector2::ZERO;
		}
	}
