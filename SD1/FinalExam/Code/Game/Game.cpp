#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cmath>
#include <cstdlib>

const float END_MOVIE_FRAME_SECONDS = 0.05f;

World* g_theWorld=nullptr;

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

void Game::initWorld()
	{
	m_theWorld=g_theWorld=new World(m_level);
	}

Game::Game()
	: m_timescale(10)
	, m_level(1)
	, m_isPaused(false)
	, m_gameState(GAME_STATE_PLAYING)
	, m_isQuittingGame(false)
	, m_currentShakeAmount(0.f)
	, m_currentShakeTimeLeft(0.f)
	, m_totalElapsedSeconds(0.f)
	, m_theWorld(nullptr)
	{
	init();
	}

Game::~Game()
	{
	destroyWorld();
	}

void Game::init()
	{
	initWorld();
	g_doShowDebugGraphics = true;
	SoundID musicId=g_theAudio->createOrGetSound("Data/Audio/Music.ogg");
	m_backgroundAudioChannel = g_theAudio->playSound(musicId,0.1f,true);
	}

void Game::reset()
	{
	destroyWorld();
	initWorld();

	}

void Game::destroyWorld()
	{
	if (m_theWorld)
		{
		delete m_theWorld;
		m_theWorld=nullptr;
		}
	}

void Game::update(float deltaSeconds)
	{
	m_totalElapsedSeconds+=deltaSeconds;
	updateScreenShakeTimer(deltaSeconds);
	if(g_theInput->wasKeyJustPressed('P'))
		g_theAudio->channelPausePlay(m_backgroundAudioChannel);
	g_theAudio->setVolume(m_backgroundAudioChannel,1.f);
// 	g_theAudio->setVolume(m_backgroundAudioChannel,calcNormalizedSineWaver(m_totalElapsedSeconds));
	if(m_theWorld)
		{
		m_theWorld->update(deltaSeconds);
		// TODO: change this to new level when game gets more advanced
		if(m_theWorld->isLevelOver())
			{
			m_isQuittingGame=true;
			g_theAudio->playSound(g_theAudio->createOrGetSound("Data/Audio/RunnerEscaped.mp3"));
			}
		// TODO: change this death state when game gets more advanced
		if(m_theWorld->isPlayerHit())
			{
			m_isQuittingGame=true;
			g_theAudio->playSound(g_theAudio->createOrGetSound("Data/Audio/GameLost.ogg"));
			}
		}

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

	m_theWorld->render();
	}


void Game::toggleDebugGraphics()
	{
	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}