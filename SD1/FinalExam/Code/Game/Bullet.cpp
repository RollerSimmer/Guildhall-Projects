#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World.hpp"
#include "Engine/Math/MathUtil.hpp"

Bullet::Bullet(const Vector2& pos,float orientationDegrees,const Rgba& color, bool canSeek, float radius) 
	: Entity(pos,ENTITY_TYPE_BULLET,false,radius,0.f,1,true)
	, m_canSeek(canSeek)
	{
	m_orientationDegrees=orientationDegrees;
	float speed=200.f;
	m_vel=calcHeadingVector();
	m_vel.setLength(speed);
	m_movingSpriteTexture=g_theRenderer->CreateOrGetTexture("Data/Images/Bullet.png");
	m_color=color;
	}

void Bullet::update(float deltaSeconds)
	{
	seek();
	move(deltaSeconds);
	}

void Bullet::seek()
	{
	if (m_canSeek)
		{
		Vector2 d=g_theWorld->m_thePlayer->getpos()-m_pos;
		m_orientationDegrees=atan2Degrees(d.y,d.x);
		float curspeed=m_vel.CalcLength();
		Vector2 newvel=calcHeadingVector();
		newvel.setLength(curspeed);
		m_vel=newvel;
		}
	}

