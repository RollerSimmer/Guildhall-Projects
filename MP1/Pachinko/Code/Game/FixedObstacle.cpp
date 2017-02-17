#include "Game/FixedObstacle.hpp"

FixedObstacle::FixedObstacle()
	{

	}

void FixedObstacle::render() const
	{

	}

bool FixedObstacle::doesCollideWith(const Projectile* projectile) const
	{
	if(projectile)
		return false;
	else
		return false;
	}

void FixedObstacle::processCollision(Projectile* projectile)
	{
	if(projectile)
		return;
	else 
		return;
	}

void FixedObstacle::pullApartFrom(Projectile* projectile)
	{
	if (projectile)
		return;
	else
		return;
	}

void FixedObstacle::transferMomentum(Projectile* projectile)
	{
	if (projectile)
		return;
	else
		return;
	}

