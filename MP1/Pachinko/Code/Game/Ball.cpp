#include "Game/Ball.hpp"
#include "Game/GameCommon.hpp"

Ball::Ball(float mass)
	: Projectile(Vector2::ZERO,mass)
	{

	}

Ball::Ball(float xball, float yball, float ballradius)\
	: Projectile(Vector2(xball,yball),ballradius,ballradius)
	, m_shape(xball,yball,ballradius)
	{

	}

Ball::~Ball()
	{

	}

void Ball::render() const
	{
	if (g_projectileRenderType == PROJ_RENDER_TYPE_SOLID)
		{
		g_theRenderer->drawShinyDisc2(m_pos.x, m_pos.y, m_cosmeticRadius, AMT_BALL_EDGES, m_color);
		g_theRenderer->drawCircle(m_pos.x, m_pos.y, m_cosmeticRadius, AMT_BALL_EDGES, Rgba::BLACK);
		}
	else if (g_projectileRenderType == PROJ_RENDER_TYPE_LIQUID)
		{
		const Rgba watercolor=Rgba(0x80104080);
		g_theRenderer->drawLiquidDisc2(m_pos.x, m_pos.y, m_cosmeticRadius, AMT_BALL_EDGES, watercolor);
		}
	}

void Ball::update(float deltaSeconds)
	{
	Projectile::update(deltaSeconds);
	}

void Ball::scaleBy(float scale)
	{
	Projectile::scaleBy(scale);
	m_shape.scaleBy(scale);
	}
