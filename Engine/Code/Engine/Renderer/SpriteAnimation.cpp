#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cassert>

SpriteAnimation::SpriteAnimation(const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex)
	: m_spriteSheet(spriteSheet)
	, m_durationSeconds(durationSeconds)
	, m_playbackMode(playbackMode)
	, m_startSpriteIndex(startSpriteIndex)
	, m_endSpriteIndex(endSpriteIndex)
	, m_elapsedSeconds(0.f)
	, m_framesPerSecond(0.f)
	, m_isFinished(false)
	, m_isPaused(false)
	, m_isPlayingForwards(true)
	{

	}

void SpriteAnimation::update(float deltaSeconds)
	{
	float ds=deltaSeconds;
	if(!isPaused())
		m_elapsedSeconds+=ds;
	cycle(deltaSeconds);
	}

AABB2 SpriteAnimation::getCurrentTexCoords() const
	{
	AABB2 result=m_spriteSheet.getTexCoordsForSpriteIndex(getCurrentFrameNumber());
	return result;
	}

Texture* SpriteAnimation::getTexture()
	{
	Texture* result=m_spriteSheet.getTexture();
	return result;
	}

void SpriteAnimation::pause()
	{
	m_isPaused=false;
	}

void SpriteAnimation::resume()
	{
	m_isPaused=true;
	}

void SpriteAnimation::reset()
	{
	m_elapsedSeconds=0.f;
	}

float SpriteAnimation::getSecondsRemaining() const
	{
	float result=m_durationSeconds-m_elapsedSeconds;
	return result;
	}

float SpriteAnimation::getFractionElapsed() const
	{
	assert(m_durationSeconds != 0.f);
	float result=m_elapsedSeconds/m_durationSeconds;
	result=clampFloat(result,0.f,1.f);
	return result;
	}

float SpriteAnimation::getFractionRemaining() const
	{
	float result=1.f-getFractionElapsed();
	return result;
	}

void SpriteAnimation::setSecondsElapsed(float secondsElapsed)
	{
	m_elapsedSeconds=secondsElapsed;
	}

void SpriteAnimation::setFractionElapsed(float fractionElapsed)
	{
	m_elapsedSeconds=m_durationSeconds*fractionElapsed;
	}

void SpriteAnimation::cycle(float deltaSeconds)
	{
	m_elapsedSeconds+=deltaSeconds;
	
	if (m_playbackMode == SPRITE_ANIM_MODE_PLAY_TO_END)
		{
		if (m_elapsedSeconds > m_durationSeconds)
			m_elapsedSeconds=m_durationSeconds;
		}
	else if (m_playbackMode == SPRITE_ANIM_MODE_LOOPING)
		{
		while(m_elapsedSeconds > m_durationSeconds)
			m_elapsedSeconds-=m_durationSeconds;
		}
	else if (m_playbackMode == SPRITE_ANIM_MODE_PINGPONG)
		{
		if(m_elapsedSeconds>m_durationSeconds)
			m_isPlayingForwards=!m_isPlayingForwards;
		while (m_elapsedSeconds > m_durationSeconds)
			m_elapsedSeconds-=m_durationSeconds;
		}
	}

int SpriteAnimation::getCurrentFrameNumber() const
	{
	int amtframes=m_spriteSheet.getNumSprites();
	float fractionElapsed=getFractionElapsed();
	if(!m_isPlayingForwards)
		fractionElapsed=1.f-fractionElapsed;
	int result=(int)(fractionElapsed*(float)amtframes);
	return result;
	}
