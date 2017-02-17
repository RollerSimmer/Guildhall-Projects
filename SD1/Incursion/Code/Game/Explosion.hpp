#pragma once

#include "Game/Entity.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include <string>

const int EXPLOSION_SPRITE_SHEET_START_INDEX = 0;
const int EXPLOSION_SPRITE_SHEET_END_INDEX = 24;
const int EXPLOSION_SPRITE_SHEET_AMT_TILES_WIDE = 5;
const int EXPLOSION_SPRITE_SHEET_AMT_TILES_HIGH = 5;
const float DEFAULT_EXPLOSION_ELEVATION = 0.5f;

class Explosion: public Entity
	{
	public:
		Explosion(int indexIntoGameArray, const SpriteSheet& explosionSpriteSheet, 
				  const Vector2& pos, float radius,float durationSeconds);
		~Explosion();
	public: 
		virtual void render() const;
		virtual void move(float deltaSeconds);
		virtual void update(float deltaSeconds);
		virtual bool isDead() const;
		virtual bool didJustDie() const;
		virtual void respawn(const Vector2& newpos);

	protected:
		const SpriteSheet& m_explosionSpriteSheet;
		SpriteAnimation* m_explosionAnim;
	};

