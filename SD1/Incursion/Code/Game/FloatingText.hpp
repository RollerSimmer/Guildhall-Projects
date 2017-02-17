#pragma once
#include "Game/Particle.hpp"

const float FLOATING_TEXT_FONT_SIZE = 15.f;
const float FLOATING_TEXT_ASPECT_RATIO = 1.f;
const float FLOATING_TEXT_STARTING_ELEVATION = 100.f;
const float FLOATING_TEXT_SPEED = 5.f;
const float FLOATING_TEXT_JITTER_RANGE_LOW = 0.01f;
const float FLOATING_TEXT_JITTER_RANGE_HIGH = 0.5f;
extern Rgba FLOATING_TEXT_DEFAULT_COLOR;
extern Rgba FLOATING_TEXT_POSITIVE_SCORE_COLOR ;
extern Rgba FLOATING_TEXT_NEGATIVE_SCORE_COLOR ;
const float FLOATING_TEXT_LIFESPAN_IN_SECONDS = 3.f;


class FloatingText: public Particle
	{
	public:
		FloatingText(int indexIntoGameArray, const Vector2& pos, const std::string text, const Rgba& color=FLOATING_TEXT_DEFAULT_COLOR, const float radius=0.f);
		virtual void render() const;
		virtual void move(float deltaSeconds);
		virtual void update(float deltaSeconds);
// 		virtual bool isDead() const;
// 		virtual bool didJustDie() const;
// 		virtual void accelerate(float deltaSeconds);
// 		virtual void respawn(const Vector2& newpos);
		void changeJitter();
		virtual float calcAgeScale() const;
		float calcHalfTextWidth() const;
		float calcTextWidth() const;
		static void initColors();
		static bool haveColorsBeenInitialized() { return s_init; }
		
	protected:
		std::string m_text;
		Vector2 m_jitterOfs;
		float m_fontHeight;
	private:
		static bool s_init;
		
	};

