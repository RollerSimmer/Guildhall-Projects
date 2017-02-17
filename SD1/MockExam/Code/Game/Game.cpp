#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cmath>
#include <cstdlib>

const float END_MOVIE_FRAME_SECONDS = 0.05f;

Map* g_theMap=nullptr;

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


void Game::startScreenShake()
	{
	m_currentShakeAmount = SCREEN_SHAKE_AMOUNT;
	m_currentShakeTimeLeft = SCREEN_SHAKE_TIME_SPAN;
	}

void Game::doScreenShake() const
	{
	float screenShakeX = GetRandomFloatInRange(-m_currentShakeAmount, m_currentShakeAmount);
	float screenShakeY = GetRandomFloatInRange(-m_currentShakeAmount, m_currentShakeAmount);
	g_theRenderer->translate2D(Vector2(screenShakeX, screenShakeY));
	}

Game::Game()
	: m_timescale(10)
	, m_isPaused(false)
	, m_gameState(GAME_STATE_PLAYING)
	, m_isQuittingGame(false)
	, m_currentShakeAmount(0.f)
	, m_currentShakeTimeLeft(0.f)
	, m_hasPlayedSound(false)
	, m_startUpSound(MISSING_SOUND_ID)
	, m_theMap(nullptr)
	{
	init();
	}

void Game::init()
	{
	g_doShowDebugGraphics = false;
	if(g_theAudio)
		m_startUpSound=g_theAudio->createOrGetSound(std::string("Data/Audio/Explosion.ogg"));
	g_theMap=m_theMap=new Map();
	}

void Game::update(float deltaSeconds)
	{
	updateScreenShakeTimer(deltaSeconds);
	m_theMap->update(deltaSeconds);
// 	if (!m_hasPlayedSound)
// 		{
// 		m_hasPlayedSound=true;
// 		g_theAudio->playSound(m_startUpSound,0.5f);
// 		}
	}

void Game::updateScreenShakeTimer(float deltaSeconds)
	{
	if (m_currentShakeTimeLeft > 0.f)
		m_currentShakeTimeLeft -= (m_currentShakeTimeLeft > deltaSeconds ? deltaSeconds : m_currentShakeTimeLeft);
	if (m_currentShakeTimeLeft == 0.f)
		m_currentShakeAmount = 0.f;
	}

void Game::render() const
	{
	g_theRenderer->setOrthoProjection(Vector2::ZERO, Vector2(g_worldWidth, g_worldHeight));
	doScreenShake();

	m_theMap->render();
	}


void Game::toggleDebugGraphics()
	{
	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}