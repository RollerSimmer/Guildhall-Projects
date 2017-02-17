#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cmath>
#include <cstdlib>
#include <sstream>

const float END_MOVIE_FRAME_SECONDS = 0.05f;

Map* g_theMap;

Rgba Game::s_defaultBgColor=Rgba::BLUE;

void Game::renderTitleBackground() const
	{
	if (m_titleBackgroundImage)
		{
		AABB2 box = *g_theWorldBounds;
		g_theRenderer->setOrthoProjection(box);
		Rgba tint=Rgba::WHITE;
		tint.setAlphaAsFloat(calcCurrentOverlayTextAlpha());
		AABB2 texcoords(Vector2(0.f,0.f),Vector2(1.f,1.f));
		g_theRenderer->drawTexturedAABB2(box,*m_titleBackgroundImage,texcoords,tint);
		g_theRenderer->setOrthoProjection(m_theMap->calcViewport());
		}
	
	}

bool Game::isQuitting()
	{
	return m_isQuittingGame;
	}

void Game::changeScoreBy(int scoreDelta)
	{
	m_score+=scoreDelta;
	if(m_score<0)
		m_score=0;
	}

void Game::startStateTransition(GameState nextState,float secondsUntilTransition)
	{
	m_nextState=nextState;
	m_nextStateCountdownSeconds=m_transitionDurationSeconds=secondsUntilTransition;
	}

Rgba Game::calcCurrentBackgroundColor() const
	{
	Rgba stateColor=calcStateBackgroundColor();
	if(m_nextStateCountdownSeconds==0.f)
		return stateColor;
	Rgba nextColor=calcAnticipatedBackgroundColor();
	float t=calcNextStateWaitFraction();
	Rgba result=interpolate(stateColor,nextColor,t);
	return result;
	}

Rgba Game::calcBackgroundColorForGameState(GameState m_state)
	{
	Rgba result=s_defaultBgColor;
	result.setAlphaAsFloat(0.f);
	switch (m_state)
		{
		case GAME_STATE_ATTRACT: 
			result.setAlphaAsFloat(0.75f);
			break;
		case GAME_STATE_GAME_OVER:
			result.setAlphaAsFloat(0.3f);
			break;
		case GAME_STATE_PAUSED:
			result.setAlphaAsFloat(0.5f);
			break;
		case GAME_STATE_PLAYING:
			// use default
			break;
		case GAME_STATE_RESTART:
			// use default
			break;
		case GAME_STATE_CONTINUE:
			// use default
			break;
		case GAME_STATE_VICTORY:
			float scale=0.5f*(1.f+sin(25.f*g_totalElapsedSecondsSinceStart));
			result = interpolate(Rgba::BLUE,Rgba::RED,scale);
			result.setAlphaAsFloat(0.3f);
			break;
		}
	return result;
	}

float Game::calcCurrentOverlayTextAlpha() const
{
	float stateAlpha=calcOverlayTextAlphaForGameState(m_state);
	if (m_nextStateCountdownSeconds==0.f)
		return stateAlpha;
	float nextAlpha=calcOverlayTextAlphaForGameState(m_nextState);
	float t=calcNextStateWaitFraction();
	float result=interpolate(stateAlpha,nextAlpha,t);
	return result;
	}

float Game::calcOverlayTextAlphaForGameState(GameState m_state)
	{
	const float DEFAULT_TEXT_OPACITY=1.f;
	float result = DEFAULT_TEXT_OPACITY;
	switch (m_state)
		{
		case GAME_STATE_ATTRACT:
			// use default
			break;
		case GAME_STATE_GAME_OVER:
			// use default
			break;
		case GAME_STATE_PAUSED:
			// use default
			break;
		case GAME_STATE_PLAYING:
			result=0.f;
			break;
		case GAME_STATE_RESTART:
			result=0.f;
			break;
		case GAME_STATE_CONTINUE:
			result=0.f;
			break;
		case GAME_STATE_VICTORY:
			// use default
			break;
		}
	return result;
	}

bool Game::wasAnyEscKeyJustPressed()
	{
	bool result = g_theInput->wasKeyJustPressed(KEYCODE_ESCAPE)
		          ||g_theInput->getXboxController(XCONTROLLER_0).isXBoxButtonPressed(XBUTTON_BACK);
	while(result)
		break;
	return result;
	}

bool Game::wasAnyPauseKeyJustPressed()
	{
	return  (   g_theInput->wasKeyJustPressed(KEYCODE_PAUSE)
			 || g_theInput->wasKeyJustPressed('P')
			 || g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_START));
	}

GameState Game::getAnticipatedGameState()
	{
	return m_nextState;
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

void Game::drawScore() const
	{
	std::stringstream scoreStream;
	scoreStream.str("");
	scoreStream<<"Score: "<<m_score;
	std::string scoreStr=scoreStream.str();
	g_theRenderer->setOrthoProjection(*g_theWorldBounds);
	float fontHeight=32.f;
	Vector2 shadowDisplacement(3.f,-4.f);
	float textY=fontHeight;
	g_theRenderer->drawTextLineWithShadow(std::string(scoreStr),
		                        Vector2(0.f,textY),fontHeight,1.f,Rgba::WHITE,Rgba(0x0000007f),
		                        shadowDisplacement);

	//print debug information
	if (g_doShowDebugGraphics)
		{
		textY += fontHeight;
		scoreStream.str("");
		int amtEntities = m_theMap->getEntityAmount();
		scoreStream<<"# of Entities: "<<amtEntities;	
		scoreStr=scoreStream.str();
		g_theRenderer->drawTextLineWithShadow(std::string(scoreStr),
			Vector2(0.f, textY), fontHeight, 1.f, Rgba::WHITE, Rgba(0x0000007f),
			shadowDisplacement);
		}
	g_theRenderer->setOrthoProjection(m_theMap->calcViewport());
	}

void Game::playSoundBasedOnNewState()
	{
	static SoundID pauseSound=g_theAudio->createOrGetSound("Data/Audio/Pause.ogg");
	const float PAUSE_SOUND_VOLUME = 10.f;

	switch (m_state)
		{		
		case GAME_STATE_ATTRACT:
		case GAME_STATE_GAME_OVER:
		case GAME_STATE_PAUSED:
		//case GAME_STATE_PLAYING:
		case GAME_STATE_RESTART:
		case GAME_STATE_CONTINUE:
		case GAME_STATE_VICTORY:
			g_theAudio->playSound(pauseSound,PAUSE_SOUND_VOLUME);
			break;
		}
	}

void Game::updateGameState(float deltaSeconds)
	{
	// TODO: add sound effects for transitions.
	m_nextStateCountdownSeconds-=deltaSeconds;
	if(m_nextStateCountdownSeconds<0.f)
		m_nextStateCountdownSeconds=0.f;
	if (m_nextStateCountdownSeconds<=0.f && m_nextState!=NEXT_GAME_STATE_KEEP_CURRENT)
		{
		m_state=m_nextState;
		switch (m_state)
			{
			case GAME_STATE_ATTRACT:
			case GAME_STATE_PAUSED:
			case GAME_STATE_PLAYING:
			case GAME_STATE_VICTORY:
				m_nextState=NEXT_GAME_STATE_KEEP_CURRENT;
				break;
			case GAME_STATE_RESTART:
				m_score=0;
				m_theMap->resetMap();
				startStateTransition(GAME_STATE_PLAYING);
				break;
			case GAME_STATE_CONTINUE:
				m_theMap->respawnTheOnlyPlayerTank();
				startStateTransition(GAME_STATE_PLAYING);
				break;
			case GAME_STATE_GAME_OVER:
				startStateTransition(GAME_STATE_ATTRACT,GAME_OVER_SCREEN_COUNTDOWN_DURATION_SECONDS);
				break;
			}
		playSoundBasedOnNewState();
		}
	}

void Game::updateGameOverMode(float deltaSeconds)
	{
	//TODO: change this after finding out how to split up updatePlayMode
	updateGameplay(deltaSeconds);
	if (wasAnyEscKeyJustPressed())
		startStateTransition(GAME_STATE_ATTRACT,1.5f);
	if (wasAnyPauseKeyJustPressed())
		startStateTransition(GAME_STATE_CONTINUE,0.75f);
	}

void Game::updatePausedMode(float /*deltaSeconds*/)
	{
	if (wasAnyEscKeyJustPressed())
		startStateTransition(GAME_STATE_ATTRACT,1.5f);
	if(wasAnyPauseKeyJustPressed())
		startStateTransition(GAME_STATE_PLAYING,0.75f);
	}

void Game::updateAttractMode(float /*deltaSeconds*/)
	{
	if (wasAnyEscKeyJustPressed())
		m_isQuittingGame=true;
	if (g_theInput->wasKeyJustPressed(KEYCODE_SPACE) 
		|| g_theInput->getXboxController(XCONTROLLER_0).wasXboxButtonJustPressed(XBUTTON_START))
		startStateTransition(GAME_STATE_RESTART,1.5f);
	}

void Game::updatePlayMode(float deltaSeconds)
	{	
	float ds=deltaSeconds;
	if (wasAnyPauseKeyJustPressed() || wasAnyEscKeyJustPressed())
		startStateTransition(GAME_STATE_PAUSED,0.1f);
	if (g_theInput->isKeyDown('F'))
		ds*=0.f;
	updateGameplay(ds);
	}

void Game::renderAttractMode() const
	{
	//renderPlayMode();
	renderTitleBackground();
	// draw black box
	drawCurrentBackgroundOrOverlay();
	// print PAUSED 
	Rgba textcolor = Rgba::WHITE;
	textcolor.setAlphaAsFloat(calcCurrentOverlayTextAlpha());
	g_theRenderer->setOrthoProjection(*g_theWorldBounds);
	float fontHeight = 72.f;
	Vector2 shadowDisplacement(3.f,-4.f);
	Vector2 textPos=g_theWorldBounds->CalcCenter();
	textPos.y += (fontHeight*0.5f);
	float aspect = 1.f;
	std::stringstream ss("");
	ss.str("");
	ss<<"Incursion\n";
	textcolor=Rgba::WHITE;
	textcolor.setAlphaAsFloat(calcCurrentOverlayTextAlpha());
	g_theRenderer->drawTextLines(ss.str(),textPos,fontHeight,aspect,textcolor);
	ss.str("");
	ss<<"by: John Anderson\nPress Start or Spacebar to begin.\n";
	textPos.y -= (fontHeight);
	fontHeight*=0.33f;
	textcolor=Rgba::WHITE;
	textcolor.setAlphaAsFloat(calcCurrentOverlayTextAlpha());
	g_theRenderer->drawTextLines(ss.str(),textPos,fontHeight,aspect,textcolor);
	ss.str("");
	ss<<"Controls: \n\tW/A/S/D or tilt left Xbox stick to move around\n";
	ss<<"\tI/J/K/L or press/tilt right Xbox stick to aim tank's gun.\n";
	ss<<"\tESC, Pause, P, Xbox Start button or Xbox Back button pauses.\n";
	textPos.x=365.f;
	textPos.y -= (fontHeight*4.f);
	fontHeight*=0.66f;
	textcolor=Rgba::LIME;
	textcolor.setAlphaAsFloat(calcCurrentOverlayTextAlpha());
	g_theRenderer->drawTextLines(ss.str(),textPos,fontHeight,aspect*0.75f,textcolor,false);
	g_theRenderer->setOrthoProjection(m_theMap->calcViewport());
	}

void Game::renderPlayMode() const
	{
	doScreenShake();
	m_theMap->render();
	drawScore();
	}

void Game::renderGameOverMode() const
	{
	renderPlayMode();
	// draw black box
	drawCurrentBackgroundOrOverlay();
	// print PAUSED 
	Rgba textcolor = Rgba::WHITE;
	textcolor.setAlphaAsFloat(calcCurrentOverlayTextAlpha());
	g_theRenderer->setOrthoProjection(*g_theWorldBounds);
	float fontHeight = 32.f;
	Vector2 shadowDisplacement(3.f,-4.f);
	Vector2 textPos=g_theWorldBounds->CalcCenter();
	textPos.y += (fontHeight*0.5f);
	float aspect = 1.f;
	float fontWidth = fontHeight/aspect;
	textPos.x -= (fontWidth*3.f);
	std::stringstream ss("");
	ss<<"You died\npress Start or P to resume.";
	if(m_nextState==GAME_STATE_ATTRACT)
		{
		ss<<"\nSeconds left: ";
		ss<<(int)m_nextStateCountdownSeconds;
		}
	g_theRenderer->drawTextLines(ss.str(),textPos,fontHeight,aspect,textcolor);
	g_theRenderer->setOrthoProjection(m_theMap->calcViewport());
	}

void Game::updateGameplay(float deltaSeconds)
{
	if (m_state==GAME_STATE_PAUSED)
		deltaSeconds *= 0.f;
	updateScreenShakeTimer(deltaSeconds);
	m_theMap->update(deltaSeconds);
	}

void Game::renderVictory() const
	{
	renderPlayMode();
	drawCurrentBackgroundOrOverlay();
	// 	bgColor = isPercentChance(50) ? Rgba::RED : Rgba::BLUE ;
	// 	bgColor.setAlphaAsFloat(0.3f);

	// print PAUSED 
	Rgba textcolor = Rgba::WHITE;
	g_theRenderer->setOrthoProjection(*g_theWorldBounds);
	float fontHeight = 128.f;
	Vector2 shadowDisplacement(3.f,-4.f);
	Vector2 textPos=g_theWorldBounds->CalcCenter();
	textPos.y += (fontHeight*0.5f);
	float aspect = 1.f;
	std::stringstream ss("");
	ss<<"You Won!!!";
	g_theRenderer->drawTextLines(ss.str(),textPos,fontHeight,aspect,Rgba::WHITE);
	g_theRenderer->setOrthoProjection(m_theMap->calcViewport());
	}

void Game::drawCurrentBackgroundOrOverlay() const
	{
	g_theRenderer->setOrthoProjection(*g_theWorldBounds);
	Rgba bgColor=calcCurrentBackgroundColor();
	AABB2 box = *g_theWorldBounds;
	g_theRenderer->drawFilledBox(box,bgColor);
	g_theRenderer->setOrthoProjection(m_theMap->calcViewport());
	}

void Game::updateVictoryMode(float /*deltaSeconds*/)
	{
	//updateGameplay(deltaSeconds);
	if(wasAnyEscKeyJustPressed() && m_state != GAME_STATE_ATTRACT)
		startStateTransition(GAME_STATE_ATTRACT);
	}

Rgba Game::calcAnticipatedBackgroundColor() const
	{
	return calcBackgroundColorForGameState(m_nextState);
	}

Rgba Game::calcStateBackgroundColor() const
	{
	return calcBackgroundColorForGameState(m_state);
	}

float Game::calcNextStateWaitFraction() const
	{
	float fracRemaining=m_nextStateCountdownSeconds/m_transitionDurationSeconds;
	fracRemaining=clampFloat(fracRemaining,0.f,1.f);
	float result=1.f-fracRemaining;
	return result;
	}

Game::Game()
	: m_timescale(10)
	, m_isPaused(false)
	, m_isQuittingGame(false)
	, m_currentShakeAmount(0.f)
	, m_currentShakeTimeLeft(0.f)
	, m_theMap(nullptr)
	, m_score(0)
	, m_state(GAME_STATE_ATTRACT)
	, m_nextState(NEXT_GAME_STATE_KEEP_CURRENT)
	{
	init();
	}

Game::~Game()
	{
	delete m_theMap;
	m_theMap=nullptr;
	}

void Game::init()
	{
	g_doShowDebugGraphics = false;
	m_theMap=new Map();
	g_theMap=m_theMap;
	m_titleBackgroundImage=g_theRenderer->CreateOrGetTexture("Data/Images/TitleBackground.png");
	}

void Game::update(float deltaSeconds)
	{
	updateGameState(deltaSeconds);
	if (g_theInput->isKeyDown('T'))
		deltaSeconds *= 0.1f;
	switch (m_state)
		{
		case GAME_STATE_PLAYING:
		case GAME_STATE_RESTART:
		case GAME_STATE_CONTINUE:
			updatePlayMode(deltaSeconds);
			break;
		case GAME_STATE_VICTORY:
			updateVictoryMode(deltaSeconds);
			break;
		case GAME_STATE_ATTRACT:
			updateAttractMode(deltaSeconds);
			break;
		case GAME_STATE_PAUSED:
			updatePausedMode(deltaSeconds);
			break;
		case GAME_STATE_GAME_OVER:
			updateGameOverMode(deltaSeconds);
			break;

		}
	}

void Game::updateScreenShakeTimer(float deltaSeconds)
	{
	if (m_currentShakeTimeLeft > 0.f)
		m_currentShakeTimeLeft -= (m_currentShakeTimeLeft > deltaSeconds ? deltaSeconds : m_currentShakeTimeLeft);
	if (m_currentShakeTimeLeft == 0.f)
		m_currentShakeAmount = 0.f;
	}

void Game::updateMap(float deltaSeconds)
	{
	m_theMap->update(deltaSeconds);
	}

void Game::render() const
	{
	g_theRenderer->setOrthoProjection(m_theMap->calcViewport());

	switch (m_state)
		{
		case GAME_STATE_ATTRACT: 
			renderAttractMode();
			break;
		case GAME_STATE_PLAYING:
		case GAME_STATE_RESTART:
		case GAME_STATE_CONTINUE:
			renderPlayMode();
			break;
		case GAME_STATE_PAUSED:
			renderPauseMode();
			break;
		case GAME_STATE_VICTORY:
			renderVictory();
			break;
		case GAME_STATE_GAME_OVER:
			renderGameOverMode();
			break;
		}
	}

void Game::toggleDebugGraphics()
	{
	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}

void Game::renderPauseMode() const
	{
	renderPlayMode();
	// draw black box
	drawCurrentBackgroundOrOverlay();
	// print PAUSED 
	Rgba textcolor=Rgba::WHITE;
	textcolor.setAlphaAsFloat(calcCurrentOverlayTextAlpha());
	g_theRenderer->setOrthoProjection(*g_theWorldBounds);
	float fontHeight = 32.f;
	Vector2 shadowDisplacement(3.f, -4.f);
	Vector2 textPos=g_theWorldBounds->CalcCenter();
	float aspect = 1.f;
	g_theRenderer->drawTextLines(std::string("PAUSED\nPress P, Pause or\nXbox Start to continue"),
		textPos, fontHeight, aspect,textcolor);
	g_theRenderer->setOrthoProjection(m_theMap->calcViewport());
	}
