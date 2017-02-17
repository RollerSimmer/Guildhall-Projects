#include "Game/FloatingText.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Rgba.hpp"

Rgba FLOATING_TEXT_DEFAULT_COLOR;
Rgba FLOATING_TEXT_POSITIVE_SCORE_COLOR;
Rgba FLOATING_TEXT_NEGATIVE_SCORE_COLOR;

bool FloatingText::s_init=false;

FloatingText::FloatingText(int indexIntoGameArray, const Vector2& pos, const std::string text, const Rgba& color/*=FLOATING_TEXT_DEFAULT_COLOR*/, const float radius/*=0.f*/)
	: Particle(indexIntoGameArray,pos,Vector2::getRandomVector(FLOATING_TEXT_SPEED),radius,FLOATING_TEXT_LIFESPAN_IN_SECONDS,color)
	, m_text(text) 
	, m_jitterOfs(Vector2::ZERO)
	, m_fontHeight(FLOATING_TEXT_FONT_SIZE)
	{
	initColors();
	m_elevation=FLOATING_TEXT_STARTING_ELEVATION;
	m_shouldDiscardOnDeath=true;
	m_shouldExplodeOnDeath=false;
	m_cosmeticRadius=m_physicsRadius=1.f;
	m_baseTexture=nullptr;
	m_primaryColor=color;
	}

void FloatingText::render() const
	{
	// TODO: draw text with text color, black shadow, and displaced by random jitter
	Vector2 jitteredPos=m_pos+m_jitterOfs;
	Rgba textcolor = m_primaryColor;
	float ageScale=calcAgeScale();
	float textDispBase=2.f;
	Vector2 shadowDisplacement(textDispBase*ageScale, -textDispBase*ageScale);
	Vector2 textPos = jitteredPos;
	textPos.y += (m_fontHeight*0.5f);
	float aspect = 1.f;
	textPos.x -= calcHalfTextWidth();
	g_theRenderer->drawTextLineWithShadow(m_text,
		textPos, m_fontHeight, aspect, m_primaryColor, Rgba(0x0000007f),
		shadowDisplacement);
	}

void FloatingText::move(float deltaSeconds)
	{
	m_pos+=m_vel*deltaSeconds;
	changeJitter();
	}

void FloatingText::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	move(deltaSeconds);
	changeAge(deltaSeconds);
	m_fontHeight = FLOATING_TEXT_FONT_SIZE*calcAgeScale();
	changeJitter();
	updateEnd(deltaSeconds);
	}

// bool FloatingText::isDead() const
// 	{
// 	return Particle::isDead();
// 	}
// 
// bool FloatingText::didJustDie() const
// 	{
// 	return Particle::didJustDie();
// 	}
// 
// void FloatingText::accelerate(float /*deltaSeconds*/)
// 	{
// 
// 	}
// 
// void FloatingText::respawn(const Vector2& /*newpos*/)
// 	{
// 
// 	}
// 
void FloatingText::changeJitter()
	{
	float jitterAmt=GetRandomFloatInRange(FLOATING_TEXT_JITTER_RANGE_LOW,FLOATING_TEXT_JITTER_RANGE_HIGH);
	m_jitterOfs = Vector2::getRandomVector(jitterAmt);
	}

float FloatingText::calcAgeScale() const
	{
	float x = m_ageSeconds / m_lifespanSeconds;
	float tinv = 1 - x;
	float result = tinv;
	result = clampFloat(result, 0.f, 1.f);
	return result;
	}

float FloatingText::calcHalfTextWidth() const
	{
	return 0.5f*calcTextWidth();
	}

float FloatingText::calcTextWidth() const
	{
	std::string textCopy = m_text;
	float textLenFloat=(float)textCopy.length()*FLOATING_TEXT_ASPECT_RATIO;
	float charwidth= m_fontHeight;
	float result=charwidth*textLenFloat;
	return result;
	}

void FloatingText::initColors()
	{
	if (!s_init)
		{
		FLOATING_TEXT_DEFAULT_COLOR = Rgba::LIME*0.5f + Rgba::WHITE*0.5f;
		FLOATING_TEXT_POSITIVE_SCORE_COLOR = Rgba::LIME;
		FLOATING_TEXT_NEGATIVE_SCORE_COLOR = Rgba::RED;
		s_init = true;
		}
	}
