#pragma once
#include "Entity.hpp"
class Bullet:
	public Entity
	{
	public:

		bool m_canSeek;

		void seek();


		Bullet(const Vector2& pos,float orientationDegrees,const Rgba& color, bool canSeek, float radius);
		virtual void update(float deltaSeconds);
	};

