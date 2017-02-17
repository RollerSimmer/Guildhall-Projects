#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"

enum SpriteAnimMode
	{
	SPRITE_ANIM_MODE_PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
	SPRITE_ANIM_MODE_LOOPING,		// Play from time=0 to end then repeat (never finish)
    SPRITE_ANIM_MODE_PINGPONG, 	// optional, play forwards, backwards, forwards...
	NUM_SPRITE_ANIM_MODES
	};

const float DEFAULT_SPRITE_ANIMATION_FRAMES_PER_SECOND = 60.f;

class SpriteAnimation
	{
	public:
		SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds,
		                SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex);

		void update(float deltaSeconds);
		AABB2 getCurrentTexCoords() const;	// Based on the current elapsed time
		Texture* getTexture();
		void pause();					// Starts unpaused (playing) by default
		void resume();				// Resume after pausing
		void reset();					// Rewinds to time 0 and starts (re)playing
		bool isFinished() const { return m_isFinished; }
		bool isPlaying() const { return !m_isPaused; }
		bool isPaused() const { return m_isPaused; }
		float getDurationSeconds() const { return m_durationSeconds; }
		float getSecondsElapsed() const { return m_elapsedSeconds; }
		float getSecondsRemaining() const;
		float getFractionElapsed() const;
		float getFractionRemaining() const;
		void setSecondsElapsed(float secondsElapsed);	    // Jump to specific time
		void setFractionElapsed(float fractionElapsed);    // e.g. 0.33f for one-third in
		int getCurrentFrameNumber() const;
		void cycle(float deltaSeconds);

	private:
		const SpriteSheet& m_spriteSheet;
		SpriteAnimMode m_playbackMode;
		float m_durationSeconds;
		float m_elapsedSeconds;
		int m_startSpriteIndex;
		int m_endSpriteIndex;
		float m_framesPerSecond;
		bool m_isPaused;
		bool m_isFinished;
		bool m_isPlayingForwards;
	};