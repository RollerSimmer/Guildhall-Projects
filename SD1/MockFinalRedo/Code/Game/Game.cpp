#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cmath>
#include <cstdlib>
#include "Engine/Core/Time.hpp"

const float END_MOVIE_FRAME_SECONDS = 0.05f;

SpriteSheet* g_sprites=nullptr;

void Game::spawnEnemies(int amt)
	{
	for(int i=0;i<amt;i++)
		spawnEnemy();
	}

int Game::countEnemies()
	{
	int result=0;
	for (Entity* e: m_entities)
		{
		if (e && e->gettype()==ENTITY::ENEMY)
			{
			result++;
			}
		}
	return result;
	}

Game::Game()
	: m_timescale(10)
	, m_isPaused(false)
	, m_gameState(GAME_STATE_PLAYING)
	, m_isQuittingGame(false)
	, m_currentShakeAmount(0.f)
	, m_trauma(0.f)
	, m_wasEnemyLimitEverReached(false)
	, m_winAnimation(nullptr)
	, m_deathAnimation(nullptr)
	,m_totalTimeSeconds(0.f)
	{
	init();
	}

void Game::init()
	{
	g_doShowDebugGraphics = false;
	m_bgtex=g_theRenderer->CreateOrGetTexture(std::string("Data/Images/Background.png"));
	g_sprites=g_theRenderer->createOrGetSpriteSheet(std::string("Data/Images/Sprites.png"),8,8);
	spawnThePlayerShip();
	spawnEnemies(AMT_INITIAL_ENEMIES);
	}

void Game::togglePause()
	{
	if(m_isPaused)
		m_isPaused = false;
	else
		m_isPaused = true;
	}

bool Game::isQuitting()
	{
	return m_isQuittingGame;
	}


void Game::updatePlayerDeath(float deltaSeconds)
	{
	if (isTraumaCritical())
		spawnDeathAnimation();
	else if (m_deathAnimation)
		{
		m_deathAnimation->update(deltaSeconds);
		if(m_deathAnimation->isDead())
			g_theApp->setIsQuitting(true);
		}
	}

void Game::spawnBullet(const Vector2& bulletpos,const Vector2& bulletvel, float bulletOrientationDegrees)
	{
	Entity* bullet=Entity::createBullet(bulletpos,bulletvel, bulletOrientationDegrees);
	if(bullet)
		m_entities.push_back(bullet);
	}

void Game::spawnEnemy()
	{
	if (!m_wasEnemyLimitEverReached)
		{
		AABB2 paddedworld=g_theWorldBounds->createdWorldExpandedBy(ENEMY_COSMETIC_RADIUS);
		Vector2 epos=paddedworld.GenerateEdgePoint(ENEMY_COSMETIC_RADIUS);
		Entity* e=Entity::createEnemy(epos);
		if (e)
			m_entities.push_back(e);
		}
	}

void Game::spawnExplosionAt(const Vector2& pos,float radius)
	{
	Entity* e = Entity::createExplosion(pos, radius);
	if(e)
		{
		m_entities.push_back(e);
		}
	}

void Game::spawnWinAnimation()
	{
	if (m_winAnimation==nullptr)
		{
		m_winAnimation=Entity::createWinAnimation();
		g_theAudio->playSound(g_theAudio->createOrGetSound(std::string("Data/Audio/Victory.mp3")));
		}
	}

void Game::spawnDeathAnimation()
	{
	if (m_deathAnimation==nullptr)
		{
		m_deathAnimation=Entity::createDeathAnimation();
		g_theAudio->playSound(g_theAudio->createOrGetSound(std::string("Data/Audio/PlayerKilled.wav")));
		}
	}

void Game::increaseTraumaBy(float tinc)
	{
	m_trauma+=tinc;
	clampTrauma();
	}

void Game::doScreenShake() const
	{
	float time=m_totalTimeSeconds;
	float screenShakeX = m_currentShakeAmount*cos(100.f*time);
	float screenShakeY = m_currentShakeAmount*sin(100.f*time);
	g_theRenderer->translate2D(Vector2(screenShakeX, screenShakeY));
	}

void Game::renderTrauma() const
	{
	AABB2 bounds=*g_theWorldBounds;
	Rgba tint = Rgba::RED;
	tint.setAlphaAsFloat(m_trauma);
	g_theRenderer->drawFilledBox(bounds,tint);
	}

void Game::renderEntities() const
	{
	for (Entity* e: m_entities)
		{
		if (e)
			{
			e->render();
			}
		}
	if(m_winAnimation)
		m_winAnimation->render();
	}

void Game::decreaseTrauma(float deltaSeconds)
	{
	m_trauma-=0.2f*deltaSeconds;
	clampTrauma();
	}

void Game::clampTrauma()
	{
	m_trauma=clampFloat(m_trauma,0.f,1.f);
	}

void Game::changeScreenShakeDueToTrauma()
	{
	m_currentShakeAmount=5.f*m_trauma;
	}

void Game::renderBackground() const
	{
	AABB2 bounds=*g_theWorldBounds;
	AABB2 texcoords(Vector2(0.f,0.f),Vector2(1.f,1.f));
	Rgba tint = Rgba::WHITE;
	g_theRenderer->drawTexturedAABB2(bounds,*m_bgtex,texcoords,tint);
	}

void Game::spawnThePlayerShip()
	{
	if (tallyPlayerShips()==0)
		{
		Vector2 spos=g_theWorldBounds->CalcCenter();
		m_entities.push_back(Entity::createShip(spos));
		}
	}

int Game::tallyPlayerShips()
	{	
	int result=0;
	for (Entity* e: m_entities)
		{
		if (e && e->gettype()==ENTITY::SHIP)
			{
			result++;
			}
		}
	return result;
	}

Entity* Game::findThePlayerShip() const
	{
	for (int i = 0; i < (int)m_entities.size() ; i++)
		{
		Entity* e=m_entities.at(i);
		if (e  && e->gettype()==ENTITY::SHIP)
			{
			return e;
			}
		}
	return nullptr;
	}

void Game::updateEntities(float deltaSeconds)
	{
	while(m_entities.size()>1)
		break;
	updateEnemyTargets();
	for (int i = 0; i < (int)m_entities.size() ; i++)
		{
		Entity*e=m_entities.at(i);
		if (e)
			e->update(deltaSeconds);
		}
	collideAllEntities();
	buryDeadEntities();
	stopEnemySpawningAtLimit();
	if(m_winAnimation==nullptr && countEnemies()==0)
		spawnWinAnimation();
	updatePlayerDeath(deltaSeconds);

	if (m_winAnimation)
		{
		m_winAnimation->update(deltaSeconds);
		if(m_winAnimation->isDead())
			g_theApp->setIsQuitting(true);
		}
	}

void Game::updateEnemyTargets()
	{
	Entity*thePlayer=findThePlayerShip();
	for (Entity* e: m_entities)
		{
		if (e&&e->gettype()==ENTITY::ENEMY)
			{
			e->setTarget(thePlayer);
			}
		}
	}

void Game::collideAllEntities()
	{
	for (int i = 0; i < (int)m_entities.size() ; i++)
		{
		Entity*e =m_entities.at(i);
		if (e)
			{
			for (int iother = i+1; iother < (int)m_entities.size() ; iother++)
				{
				if (iother!=i)
					{
					Entity*other=m_entities.at(iother);
					if (other)
						{
						collideTwoEntities(*e,*other);
						}
					}
				}
			}
		}
	}

void Game::buryDeadEntities()
	{
	std::vector<int> burylist;
	for (int i=0;i<(int)m_entities.size();i++)
		{
		Entity* e = m_entities.at(i);
		if (e && e->isDead())
			{
			burylist.push_back(i);
			}
		}
	while (!burylist.empty())
		{
		int i=burylist.back();
		burylist.pop_back();
		m_entities.erase(m_entities.begin()+i);
		}
	}

void Game::stopEnemySpawningAtLimit()
	{
	if(countEnemies()>=MAX_ENEMIES)
		{ 
		m_wasEnemyLimitEverReached=true;
		}
	}

void Game::update(float deltaSeconds)
	{
	if(g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_START))
		togglePause();
	if(m_isPaused)
		deltaSeconds=0.f;
	m_totalTimeSeconds+=deltaSeconds;
	decreaseTrauma(deltaSeconds);
	changeScreenShakeDueToTrauma();
	updateEntities(deltaSeconds);
	g_theAudio->update();
	}

void Game::render() const
	{
	g_theRenderer->setOrthoProjection(Vector2::ZERO,Vector2(g_worldWidth,g_worldHeight));
	doScreenShake();

	renderBackground();
	renderEntities();

	// keep trauma rendering last!
	renderTrauma();
	}

void Game::toggleDebugGraphics()
	{
	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}