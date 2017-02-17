#define WIN32_LEAN_AND_MEAN
#include "Game/App.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtil.hpp"

InputSys* g_theInput=nullptr;
Renderer* g_theRenderer=nullptr;
AABB2* g_theWorldBounds=nullptr;
Game* g_theGame=nullptr;
AudioSystem* g_theAudioSys=nullptr;

bool g_doShowDebugGraphics=false;

App::App()
	: m_game(nullptr)
	, m_isQuitting(false)
	, m_pauseState(false)
	, m_slowmoState(false)
	, m_pauseFactor(1.f)
	{
	g_theInput = new InputSys();
	g_theRenderer=new Renderer();
	g_theAudioSys=new AudioSystem();
	float pad=0.f;
	g_theWorldBounds=new AABB2(pad,pad,g_worldWidth-pad,g_worldHeight-pad);
	m_game = g_theGame = new Game();
	}

App::~App()
	{
	delete m_game;
	m_game=nullptr;
	delete g_theWorldBounds;
	g_theWorldBounds=nullptr;
	delete g_theAudioSys;
	g_theAudioSys=nullptr;
	delete g_theRenderer;
	g_theRenderer=nullptr;
	delete g_theInput;
	g_theInput=nullptr;
	}

//-----------------------------------------------------------------------------------------------
// This function will be removed once we add in an Application class
//
void App::update(float deltaSeconds)
	{
	if (g_theInput)
		g_theInput->update(deltaSeconds);
	if(g_theAudioSys)
		g_theAudioSys->update();
	if(m_game)
		m_game->update(deltaSeconds);
	if (g_theInput->isKeyDown(KEYCODE_ESCAPE) || g_theInput->getXboxController(XCONTROLLER_0).isXBoxButtonPressed(XBUTTON_BACK))
		this->setIsQuitting(true);
	}
	
//-----------------------------------------------------------------------------------------------
void App::runFrame()
{
	beginFrame();

	static double timeOfLastRunFrame=GetCurrentTimeSeconds();
	double timeNow = GetCurrentTimeSeconds();
	double deltaSeconds=timeNow-timeOfLastRunFrame;
	deltaSeconds = minOf(deltaSeconds,MAX_FRAME_DURATION_SECONDS);
	timeOfLastRunFrame=timeNow;

	//DebuggerPrintf("time: %f; time before: %f; deltaSeconds: %f\n",(float)timeOfLastRunFrame,(float)timeNow,(float)deltaSeconds);

	update((float) deltaSeconds);
	render();
	endFrame();
	}

void App::registerMouseMove(unsigned short mouseflags,const IntVector2& mousepos)
	{
	g_theInput->registerMouseMove(mouseflags,mousepos);
	}

void App::initMousePosition(unsigned short mouseflags,const IntVector2& mousepos)
	{
	g_theInput->initMousePosition(mouseflags,mousepos);
	}

void App::registerMouseButtonDown(unsigned short mouseflags,const IntVector2& mousepos)
	{
	g_theInput->registerMouseButtonDown(mouseflags,mousepos);
	}

void App::registerMouseButtonUp(unsigned short mouseflags,const IntVector2& mousepos)
	{
	g_theInput->registerMouseButtonUp(mouseflags,mousepos);
	}

void App::registerMouseDoubleClick(unsigned short mouseflags,const IntVector2& mousepos)
	{
	g_theInput->registerMouseDoubleClick(mouseflags,mousepos);
	}

void App::stabilizeMousePos()
	{
	g_theInput->stabilizeMousePos();
	}

void App::resetMouseButtons()
	{
	g_theInput->resetMouseButtons();
	}

void App::onGainedFocus()
	{
	DebuggerPrintf("Gained focus\n");
	if(g_theInput)
		g_theInput->onGainedFocus();
	}

void App::onLostFocus()
	{
	DebuggerPrintf("Lost focus\n");
	if(g_theInput)
		g_theInput->onLostFocus();
	}

void App::registerKeyDown(unsigned char ikey)
	{
	if(g_theInput)
		g_theInput->registerKeyDown(ikey);
	}

void App::registerKeyUp(unsigned char ikey)
	{
	if(g_theInput)
		g_theInput->registerKeyUp(ikey);
	}

bool App::isKeyDown(unsigned char ikey)
	{
	if(g_theInput)
		return g_theInput->isKeyDown(ikey);
	else
		return false;
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
void App::render() const
	{
	m_game->render();
//	drawWorldBounds();
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
