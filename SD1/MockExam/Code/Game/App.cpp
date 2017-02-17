#define WIN32_LEAN_AND_MEAN
#include "Game/App.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

InputSys* g_theInput=nullptr;
Renderer* g_theRenderer=nullptr;
AABB2* g_theWorldBounds=nullptr;
AudioSystem* g_theAudio=nullptr;
SpriteSheet* g_allSprites=nullptr;

bool g_doShowDebugGraphics=true;

App::App()
	: m_game(nullptr)
	, m_isQuitting(false)
	, m_pauseState(false)
	, m_slowmoState(false)
	, m_pauseFactor(1.f)
	{
	g_theInput = new InputSys();
	g_theAudio=new AudioSystem();
	g_theRenderer=new Renderer();
	g_allSprites=g_theRenderer->createOrGetSpriteSheet("Data/Images/Sprites.png",SPRITE_SHEET_AMT_SPRITES_WIDE,SPRITE_SHEET_AMT_SPRITES_TALL);
	float pad=0.f;
	g_theWorldBounds=new AABB2(pad,pad,g_worldWidth-pad,g_worldHeight-pad);
	m_game = new Game();
	}

App::~App()
	{
	delete g_theAudio;
	g_theAudio=nullptr;
	delete g_theRenderer;
	g_allSprites=nullptr;
	delete g_allSprites;
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
	if (g_theInput->isKeyDown(KEYCODE_ESCAPE) || g_theInput->getXboxController(XCONTROLLER_0).isXBoxButtonPressed(XBUTTON_BACK))
		this->setIsQuitting(true);
	}
	

//-----------------------------------------------------------------------------------------------
void App::RunFrame()
	{
	beginFrame();

	static double timeOfLastRunFrame=GetCurrentTimeSeconds();
	double timeNow = GetCurrentTimeSeconds();
	double deltaSeconds=timeNow-timeOfLastRunFrame;
	timeOfLastRunFrame=timeNow;

	DebuggerPrintf("time: %f; time before: %f; deltaSeconds: %f",(float)timeOfLastRunFrame,(float)timeNow,(float)deltaSeconds);

	Update((float) deltaSeconds);
	Render();
	endFrame();
	}

void App::registerKeyDown(unsigned char ikey)
	{
	g_theInput->registerKeyDown(ikey);
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
	m_isQuitting=m_isQuitting||m_game->isQuitting();
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
void App::Render() const
{
	g_theRenderer->clearScreen(0.0f, 0.1f, 0.1f);
	m_game->render();
	drawWorldBounds();
	}


void App::drawWorldBounds() const
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

