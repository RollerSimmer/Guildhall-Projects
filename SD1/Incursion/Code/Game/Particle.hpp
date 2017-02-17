#pragma once
#include "Game/Entity.hpp"
#include "Engine/Renderer/Rgba.hpp"

class Particle : public Entity
	{
	public:
		Particle(int indexIntoGameArray, const Vector2& pos, const Vector2& vel, float radius, float lifespanSeconds, const Rgba& color);
		virtual void render() const;
			
		virtual void update(float deltaSeconds);
		virtual void accelerate(float deltaSeconds);
		virtual void move(float deltaSeconds);
		virtual bool isDead() const;
		virtual bool justDied() const;
		void changeAge(float deltaSeconds);
		virtual float calcAgeScale() const;
	protected:
		float m_lifespanSeconds;
		float m_ageSeconds;

	private:
	};

