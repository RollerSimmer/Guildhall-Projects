#pragma once
#include "Entity.hpp"

namespace PLAYER_TANK
	{
	const float SPEED=250.f;
	}


class PlayerTank:	public Entity
	{
	public:
		PlayerTank(const Vector2& pos,float radius,float speed,int hpCapacity);

		virtual void render() const;
		virtual void update(float deltaSeconds);

		void updateInput(float deltaSeconds);
	};

