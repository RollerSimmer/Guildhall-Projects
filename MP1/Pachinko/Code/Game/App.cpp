#define WIN32_LEAN_AND_MEAN
#include "Game/App.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/GameCommon.hpp"

InputSys* g_theInput=nullptr;
Renderer* g_theRenderer=nullptr;
AABB2* g_theWorldBounds=nullptr;

bool g_doShowDebugGraphics=false;

float g_worldGravitationalAcceleration = 9.8f * (WORLD_SIZE / PACHINKO_MACHINE_SIZE_IN_METERS);
float g_projectileTerminalSpeed = 10.f * (WORLD_SIZE / PACHINKO_MACHINE_SIZE_IN_METERS);
float g_projectileElasticity = 0.9f;
float g_obstacleElasticity = 0.9f;
ProjectileRenderType g_projectileRenderType = PROJ_RENDER_TYPE_SOLID;

App::App()
	: m_game(nullptr)
	, m_isQuitting(false)
	, m_frameSeconds(FRAME_SECONDS)
	, m_pauseState(false)
	, m_slowmoState(false)
	, m_pauseFactor(1.f)
	{
	g_theInput = new InputSys();
	float pad=0.f;
 	g_theWorldBounds=new AABB2(pad,pad,WORLD_WIDTH-pad,WORLD_HEIGHT-pad);
	m_game = new Game();
	g_theRenderer=new Renderer();
	g_worldGravitationalAcceleration = 9.8f*(WORLD_SIZE / PACHINKO_MACHINE_SIZE_IN_METERS);
	g_projectileTerminalSpeed = 10.f*(WORLD_SIZE / PACHINKO_MACHINE_SIZE_IN_METERS);
	g_projectileElasticity = 0.9f;
	g_obstacleElasticity = 0.9f;
	}

App::~App()
	{
	delete g_theRenderer;
	g_theRenderer=nullptr;
	delete m_game;
	m_game=nullptr;
	delete g_theWorldBounds;
	g_theWorldBounds=nullptr;
	delete g_theInput;
	g_theInput=nullptr;
	}

//-----------------------------------------------------------------------------------------------
// This function will be removed once we add in an Application class
//
void App::Update(float deltaSeconds)
	{
	if (g_theInput)
		g_theInput->update(deltaSeconds);
	if(m_game)
		m_game->update(deltaSeconds);
	}
	

//-----------------------------------------------------------------------------------------------
void App::RunFrame()
	{
	beginFrame();

	static double timeOfLastRunFrame = GetCurrentTimeSeconds();
	double timeNow = GetCurrentTimeSeconds();
	double deltaSeconds = timeNow - timeOfLastRunFrame;
	timeOfLastRunFrame = timeNow;
	Update((float)deltaSeconds);
	Render();
	endFrame();
	}

void App::registerKeyDown(unsigned char ikey)
	{
	g_theInput->registerKeyDown(ikey);

	if(ikey == KEYCODE_ESCAPE)
		{
		this->setIsQuitting(true);
		}
	}

void App::registerKeyUp(unsigned char ikey)
	{
	g_theInput->registerKeyUp(ikey);
	}

bool App::isKeyDown(unsigned char ikey)
	{
	return g_theInput->isKeyDown(ikey);
	}

void App::setIsQuitting(bool quitstate)
	{
	m_isQuitting=quitstate;
	}

void App::beginFrame()
	{
	if(g_theInput)
		g_theInput->beginFrame();
	}

bool App::isQuitting()
	{
	return m_isQuitting;
	}


void App::endFrame()
	{
	if(g_theInput)
		g_theInput->endFrame();
// 	if(g_theRenderer)
// 		g_theRenderer->endFrame();
// 	if(g_theAudio)
// 		g_theAudio->endFrame();
	}


//-----------------------------------------------------------------------------------------------
// This function will be removed once we add in an Application class
//
void App::Render()
	{
	g_theRenderer->clearScreen(0.0f, 0.0f, 1.0f);
	m_game->render();
	drawWorldBounds();
	}

void App::toggleSlowmo()
	{
	m_slowmoState=!m_slowmoState;
	if(m_slowmoState)
		m_frameSeconds=0.1f*FRAME_SECONDS;
	else
		m_frameSeconds = FRAME_SECONDS;
	}

void App::drawWorldBounds()
	{
	float left,top,right,bottom;
 	g_theWorldBounds->m_mins.GetXY(left, top);
	g_theWorldBounds->m_maxs.GetXY(right, bottom);
	g_theRenderer->drawBox(left,top,right,bottom);
	}

void App::togglePause()
	{
	m_pauseState=!m_pauseState;
	if (m_pauseState)
		m_pauseFactor=0.f;
	else
		m_pauseFactor=1.f;
	}

