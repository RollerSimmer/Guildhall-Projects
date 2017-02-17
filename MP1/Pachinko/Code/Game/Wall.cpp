#include "Game/Wall.hpp"
#include "Game/Projectile.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/PhysicsUtil.hpp"
#include "Engine/Math/MathUtil.hpp"

Wall::Wall(float xcenter,float ycenter,float xradius,float yradius) 
	: m_shape(Vector2(xcenter,ycenter),xradius,yradius)
	{
	
	}


Wall::~Wall()
	{

	}

void Wall::render() const
	{
	float l,t,r,b;
	AABB2 shape=m_shape;
	shape.getBounds(l,t,r,b);
	g_theRenderer->drawFilledBox(m_shape,Rgba::GRAY);
	g_theRenderer->drawBox(l,t,r,b,Rgba::BLACK);
	}

bool Wall::doesCollideWith(const Projectile* projectile) const
	{
	float projrad=projectile->getPhysicsRadius();
	Vector2 projpos=projectile->getpos();
	Disc2 projDisc=Disc2(projpos,projrad);
	return m_shape.doesOverlapWith(projDisc);
	}

void Wall::processCollision(Projectile* projectile)
	{
	pullApartFrom(projectile);
	transferMomentum(projectile);
	}

void Wall::pullApartFrom(Projectile* projectile)
	{
	float px, py;
	Vector2 ppos=projectile->getpos();
	ppos.GetXY(px, py);
	float l, t, r, b;
	float bx,by;
	Vector2 center = m_shape.CalcCenter();
	center.GetXY(bx,by);
	m_shape.getBounds(l, t, r, b);
	float radius = projectile->getPhysicsRadius();
	float ox, oy;
	int zone = calcProjectileZone(projectile);
		// 0 1 2
		// 3 4 5
		// 6 7 8
	Vector2 distVector;
	if (zone == 4)
		{
		distVector=Vector2(px-bx,py-by);
		float halfwidth  = m_shape.calcWidth()*0.5f;
		float halfheight = m_shape.calcHeight()*0.5f;
		if (halfwidth != 0.f && halfheight != 0.f)
			{
			float xdist = abs(px - bx) / halfwidth;
			float ydist = abs(py - by) / halfheight;
			if (xdist > ydist)
				distVector.setLengthX(halfwidth + radius);
			else
				distVector.setLengthY(halfheight + radius);
			ppos = center + distVector;
			projectile->setpos(ppos);
			}
		return;
		}
	ox = clampFloat(px, l, r);
	oy = clampFloat(py, t, b);
	distVector = Vector2(px-ox,py-oy);
	float dist=distVector.CalcLength();
	if (dist < radius)
		{
		distVector.SetLength(radius);
		ppos=Vector2(ox,oy)+distVector;
		projectile->setpos(ppos);
		}
	}

void Wall::transferMomentum(Projectile* projectile)
	{
	float px, py;
	Vector2 ppos = projectile->getpos();
	ppos.GetXY(px, py);
	float l, t, r, b;
	m_shape.getBounds(l, t, r, b);
	float ox, oy;
	ox = clampFloat(px, l, r);
	oy = clampFloat(py, t, b);
	Vector2 pvel=projectile->getVelocity();
	Vector2 ovel=Vector2::ZERO;
	float pmass=projectile->getmass();
	transferMomentumGeneric(INFINITE_MASS,ovel,Vector2(ox,oy),g_obstacleElasticity,
							pmass,pvel,projectile->getpos(),g_projectileElasticity);
	projectile->setVelocity(pvel);
	}

int Wall::calcProjectileZone(Projectile* projectile)
	{
	float px, py;
	projectile->getpos().GetXY(px, py);
	float l, t, r, b;
	m_shape.getBounds(l, t, r, b);
	int xzone, yzone, zone;
	if (px < l)       xzone = 0;
	else if (px > r)  xzone = 2;
	else              xzone = 1;
	if (py < t)       yzone = 0;
	else if (py > b)  yzone = 2;
	else              yzone = 1;
	zone = yzone * 3 + xzone;         // 0 1 2
									  // 3 4 5
									  // 6 7 8	
	return zone;
	//TODO: enumerate zone values
	}
