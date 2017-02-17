#include "Game/Explosion.hpp"
#include "Game/GameCommon.hpp"

const float EXPLOSION_STARTING_SPEED = 0.f;

Explosion::Explosion(int indexIntoGameArray, const SpriteSheet& explosionSpriteSheet,
					 const Vector2& pos,float radius,float durationSeconds) 
	: Entity(indexIntoGameArray,ENTITY_TYPE_EXPLOSION,pos,radius,EXPLOSION_STARTING_SPEED)
	, m_explosionAnim(nullptr)
	, m_explosionSpriteSheet(explosionSpriteSheet)
	{
	m_explosionAnim = new SpriteAnimation(m_explosionSpriteSheet,durationSeconds,SPRITE_ANIM_MODE_PLAY_TO_END,EXPLOSION_SPRITE_SHEET_START_INDEX,EXPLOSION_SPRITE_SHEET_START_INDEX);
	m_explosionSize = m_cosmeticRadius;
	m_shouldExplodeOnDeath = false;
	m_shouldDiscardOnDeath = true;
	m_hasHealthBar = false;
	m_orientationDegrees = 0.f;
	m_elevation = DEFAULT_EXPLOSION_ELEVATION;
	m_canOverlap=true;	
	}

Explosion::~Explosion()
	{
	delete m_explosionAnim;
	}

void Explosion::render() const
	{
	Texture* texture = m_explosionSpriteSheet.getTexture();
	AABB2 bounds = calcCosmeticBoundingBox();
	AABB2 texcoords = m_explosionAnim->getCurrentTexCoords();
	g_theRenderer->blendFunc(GL_SRC_ALPHA, GL_ONE);
	g_theRenderer->drawTexturedAABB2(bounds,*texture,texcoords,m_primaryColor,m_orientationDegrees);
	g_theRenderer->blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

void Explosion::move(float /*deltaSeconds*/)
	{

	}

void Explosion::update(float deltaSeconds)
	{
	updateStart(deltaSeconds);
	m_explosionAnim->update(deltaSeconds);
	updateEnd(deltaSeconds);
	}

bool Explosion::isDead() const
	{
	float threshold = 0.0001f;
	bool result = m_explosionAnim->getFractionRemaining() <= threshold;
	return result;
	}

bool Explosion::didJustDie() const
	{
	return isDead();
	}

void Explosion::respawn(const Vector2& /*newpos*/)
	{

	}
