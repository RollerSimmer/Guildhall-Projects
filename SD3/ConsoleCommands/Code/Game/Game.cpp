#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <cmath>
#include <cstdlib>

const float END_MOVIE_FRAME_SECONDS = 0.05f;


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
	{
	init();
	}

void Game::init()
	{
	g_doShowDebugGraphics = false;
	}

void Game::update(float deltaSeconds)
	{
	updateScreenShakeTimer(deltaSeconds);
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

// 	g_theRenderer->setOrthoProjection(Vector2::ZERO, Vector2(g_worldWidth, g_worldHeight));
// 	doScreenShake();
// 
// 	Texture* testTexture = g_theRenderer->CreateOrGetTexture( "Data/Images/Test_StbiAndDirectX.png" );
// 	AABB2 bounds( 100.f, 100.f, 500.f, 500.f );
// 	AABB2 texcoords(Vector2(0.f,0.f),Vector2(1.f,1.f));
// 	Rgba tint = Rgba::WHITE;
// 	g_theRenderer->drawTexturedAABB2(bounds,*testTexture,texcoords,tint );
	}


void Game::toggleDebugGraphics()
	{
	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}