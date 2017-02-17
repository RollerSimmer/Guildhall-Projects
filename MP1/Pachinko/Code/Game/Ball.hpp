#pragma once
#include "Game/Projectile.hpp"
#include "Engine/Math/Disc2.hpp"

const int AMT_BALL_EDGES=24;

class Ball : public Projectile
	{
	public:
		Ball(float mass=1.f);
		Ball(float xball,float yball,float ballradius);
		~Ball();
	public:
		virtual void render() const;
		virtual void update(float deltaSeconds);
		virtual void scaleBy(float scale);
	protected:
		Disc2 m_shape;
	};

