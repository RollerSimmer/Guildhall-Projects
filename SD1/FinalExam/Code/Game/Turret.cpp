#include "Game/Turret.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "GameCommon.hpp"
#include "Game/World.hpp"


Turret::Turret(const Vector2& pos) 
	: Entity(pos,ENTITY_TYPE_TURRET,true,50.f,100.f,1,false)
	, m_cooldownSeconds(0.f)
	, m_mountedSpriteTexture(nullptr)
	, m_canAim(false)
	, m_doesShootSeekers(false)
	,m_shootingIntervalSeconds(1.5f)
	{
	m_movingSpriteTexture = g_theRenderer->CreateOrGetTexture("Data/Images/Turret.png");
	m_mountedSpriteTexture=g_theRenderer->CreateOrGetTexture("Data/Images/TurretBase.png");
	m_orientationDegrees=GetRandomFloatInRange(0.f,360.f);
	m_vel=Vector2::ZERO;
	m_color=Rgba(153,102,53,255);
	}

Turret* Turret::createAimingTurret(const Vector2& pos)
	{
	Turret* t=new Turret(pos);
	t->m_color=Rgba(51,153,102,255);
	t->m_canAim=true;
	t->m_shootingIntervalSeconds=1.5f;
	return t;
	}	


Turret* Turret::createHomingTurret(const Vector2& pos)
	{
	Turret* t=new Turret(pos);
	t->m_color=Rgba(102,102,153,255);
	t->m_canAim=true;
	t->m_doesShootSeekers=true;
	t->m_shootingIntervalSeconds=3.f;
	return t;
	}

void Turret::update(float deltaSeconds)
	{
	m_cooldownSeconds-=deltaSeconds;
	bool haslos=doesHaveLineOfSightToPlayer();
	aim();
	if (m_cooldownSeconds<=0.f)
		{
		if(!m_canAim || haslos)
			shoot();
		m_cooldownSeconds=m_shootingIntervalSeconds;
		}
	}

void Turret::render() const
	{
	if (g_doShowDebugGraphics)
		{
		drawGraphicsShell();
		drawPhysicsShell();
		drawVelocityVector();
		}
	drawMountedSprite();
	drawDarkMovingSprite();

	}

void Turret::shoot()
	{
	Vector2 bpos=m_pos+calcHeadingVector()*m_cosmeticRadius;

	float bradRatio=1.3f;
	float brad=5.f;
	if(m_canAim) 
		brad*=bradRatio;
	if(m_doesShootSeekers)
		brad*=bradRatio;

	g_theWorld->spawnBullet(bpos, m_orientationDegrees, m_color, m_doesShootSeekers, brad);
	SoundID sfxId=g_theAudio->createOrGetSound("Data/Audio/QuickShot.ogg");
	g_theAudio->playSound(sfxId,1.f,false);
	}

void Turret::drawMountedSprite() const
	{
	if (m_mountedSpriteTexture)
		{
		Rgba color = m_color;
		AABB2 bounds(m_pos,m_cosmeticRadius,m_cosmeticRadius);
		g_theRenderer->drawTexturedAABB2(bounds,*m_mountedSpriteTexture,Vector2(0.f,1.f),Vector2(1.f,0.f),color,
										 0.f);
		}

	}

void Turret::aim()
	{
	if(m_canAim)
		{
		Vector2 d=g_theWorld->m_thePlayer->getpos()-m_pos;
		m_orientationDegrees=atan2Degrees(d.y,d.x);
		}
	}

void Turret::drawDarkMovingSprite() const
	{
	if (m_movingSpriteTexture)
		{
		Rgba color = m_color;
		color.scaleRGB(0.5f);
		AABB2 bounds(m_pos,m_cosmeticRadius,m_cosmeticRadius);
		g_theRenderer->drawTexturedAABB2(bounds,*m_movingSpriteTexture,Vector2(0.f,1.f),Vector2(1.f,0.f),color,
										 m_orientationDegrees);
		}
	}

bool Turret::doesHaveLineOfSightToPlayer()
	{
	return g_theWorld->doesTurretHaveLineOfSightToPlayer(*this);
	}
