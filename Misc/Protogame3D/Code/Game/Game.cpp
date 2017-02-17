#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/File/FileUtils.hpp"
#include "Engine/Core/EngineBase.hpp"
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <sstream>
/*#include <gl/GL.h>*/

//TODO: center the loaded chunks on the player and not the camera

const float END_MOVIE_FRAME_SECONDS = 0.05f;

SpriteSheet* g_tileAtlas=nullptr;
World* g_theWorld=nullptr;
BitmapFont* g_theFont=nullptr;
DebugMessagePrinter* g_theDebugMessagePrinter=nullptr;

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

void Game::addDebugDotAt(const Vector3& pos)
	{
	m_debugDotPoints.push_back(pos);
	}

void Game::doUnitTests()
	{


	}

void Game::displayHUD() const
	{
	goIntoTextRenderMode();

	//TODO: add a Heads-UP Display here if needed.

	goIntoSceneRenderMode();
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

void Game::goIntoSceneRenderMode() const
	{

	g_theRenderer->setPerspectiveProjection(m_cam.m_fovVerticalDegrees,
											m_cam.m_aspectRatio,m_cam.m_nearDistance,
											m_cam.m_farDistance);

	g_theRenderer->rotate3D(-90.f,Vector3::X_AXIS_VECTOR);
	g_theRenderer->rotate3D(90.f,Vector3::Z_AXIS_VECTOR);

	g_theRenderer->rotate3D(-m_cam.m_rollDegreesAboutX,Vector3::X_AXIS_VECTOR);
	g_theRenderer->rotate3D(-m_cam.m_pitchDegreesAboutY,Vector3::Y_AXIS_VECTOR);
	g_theRenderer->rotate3D(-m_cam.m_yawDegreesAboutZ,Vector3::Z_AXIS_VECTOR);

	g_theRenderer->translate3D(-m_cam.m_pos);

	g_theRenderer->enableBackFaceCulling(true);
	g_theRenderer->enableDepthTesting(true);
	}

void Game::goIntoTextRenderMode() const
	{
	g_theRenderer->setOrthoProjection(*g_theWorldBounds);

	g_theRenderer->enableBackFaceCulling(false);
	g_theRenderer->enableDepthTesting(false);
	}

void Game::updateInput(float deltaSeconds)
	{
	updateRotationInput(deltaSeconds);
	updatePositionInput(deltaSeconds);

 	}

std::string Game::makeDebugLines() const
	{
	std::stringstream ss;
	ss.str("");
	//ss<<"Mouse Displacement: "<<g_theInput->calcMouseMoveDisplacement().asString()<<std::endl;
	ss<<"Camera: "<<m_cam.asString()<<std::endl;

	ss<<"Camera mode: "<<m_cam.getModeName()<<std::endl;
	ss<<"W,S moves camera forward and backward"  <<std::endl;
	ss<<"A,D moves camera left and right"        <<std::endl;
	ss<<"ESC leaves the application"             ;
	std::string result= ss.str();
	return result;
	}

void Game::updateRotationInput(float deltaSeconds)
	{
	float ROTATION_SPEED = 45.f;
	float rotAmt=ROTATION_SPEED*deltaSeconds;
	if (g_theInput->isKeyDown('I'))
		m_cam.m_pitchDegreesAboutY+=rotAmt;
	if (g_theInput->isKeyDown('K'))
		m_cam.m_pitchDegreesAboutY-=rotAmt;
	if (g_theInput->isKeyDown('J'))
		m_cam.m_yawDegreesAboutZ+=rotAmt;
	if (g_theInput->isKeyDown('L'))
		m_cam.m_yawDegreesAboutZ-=rotAmt;
	if (g_theInput->isKeyDown('U'))
		m_cam.m_rollDegreesAboutX+=rotAmt;
	if (g_theInput->isKeyDown('O'))
		m_cam.m_rollDegreesAboutX-=rotAmt;
	updateCameraFromMouse();
	m_cam.m_pitchDegreesAboutY=clampFloat(m_cam.m_pitchDegreesAboutY,-89.99999f,89.999999f);
	m_cam.m_yawDegreesAboutZ=fmod(m_cam.m_yawDegreesAboutZ,360.f);
	m_cam.m_rollDegreesAboutX=fmod(m_cam.m_rollDegreesAboutX,360.f);
	}

void Game::updatePositionInput(float deltaSeconds)
	{
	float moveAmt=deltaSeconds*MOVE_SPEED;
	Vector3 moveDisplacement=Vector3::ZERO;
	Vector3 forward=m_cam.getForward();
	Vector3 left=m_cam.getLeftXY();
	Vector3 up=m_cam.m_mode==CAMERA::NO_CLIP ? Vector3::Z_AXIS_VECTOR : Vector3::ZERO;
	if (g_theInput->isKeyDown('W'))
		moveDisplacement+=forward;
	if (g_theInput->isKeyDown('S'))
		moveDisplacement-=forward;
	if (g_theInput->isKeyDown('A'))
		moveDisplacement+=left;
	if (g_theInput->isKeyDown('D'))
		moveDisplacement-=left;
	if (g_theInput->isKeyDown('Q'))
		moveDisplacement+=up;
	if (g_theInput->isKeyDown('E'))
		moveDisplacement-=up;
	if (g_theInput->isKeyDown(KEYCODE_SHIFT) && m_cam.m_mode==CAMERA::NO_CLIP)
		moveAmt*=10.f;
	// 	if(moveDisplacement==Vector3::ZERO)
	// 		return;

	moveDisplacement*=moveAmt;
	m_cam.m_pos+=moveDisplacement;
	}

void Game::updateCameraFromMouse()
	{
	if (!g_theInput->doesHaveFocus())
		return;
	if (m_cam.m_mode!=CAMERA::NO_CLIP)
		return;
	IntVector2 screenCenter = g_theInput->getWindowCenter();
	IntVector2 mousepos = g_theInput->getCursorPos();
	IntVector2 mousedelta = mousepos-screenCenter;
	if (mousedelta==IntVector2::ZERO)
		return;

	g_theInput->centerCursorPos();

	float mouseMoveX=(float)mousedelta.x;
	float mouseMoveY=(float)mousedelta.y;
	float mouseSensitivityX=0.25f;
	float mouseSensitivityY=0.25f;
	float invertYMulitiplier=-1.f;
	if (mouseMoveX!=0)
		m_cam.m_yawDegreesAboutZ-=mouseMoveX*mouseSensitivityX;
	if (mouseMoveY!=0)
		m_cam.m_pitchDegreesAboutY-=mouseMoveY*mouseSensitivityY*invertYMulitiplier;
	}

void Game::drawDebugDots() const
	{
	if (m_debugDotPoints.size()==0)
		return;
	Rgba color=Rgba::PURPLE;
	float width=15.f;
	g_theRenderer->drawDots(m_debugDotPoints, color, width);
	g_theRenderer->enableDepthTesting(false);
	//g_theRenderer->drawLine3(LineSegment3(m_debugDotPoints.at(0),m_debugDotPoints.at(m_debugDotPoints.size()-1)),Rgba::YELLOW,5.f);
	g_theRenderer->enableDepthTesting(true);
// 	g_theRenderer->drawLine3(LineSegment3(Vector3(0.f,0.f,0.f),Vector3(500.f,500.f,500.f)),Rgba::YELLOW,5.f);
	}

void Game::displayGameDebugInfo() const
	{
	if (g_theFont == nullptr)
		{
		g_theFont = new BitmapFont(g_theRenderer,"Data/Fonts/SquirrelFixedFont.png",AMT_FONT_SHEET_TILES_X,AMT_FONT_SHEET_TILES_Y);
		if (g_theFont == nullptr) // still!?
			return;
		}
	if (g_theDebugMessagePrinter==nullptr)
		{
		g_theDebugMessagePrinter=new DebugMessagePrinter(g_theRenderer,g_theFont);
		if(g_theDebugMessagePrinter==nullptr) return;
		}

	goIntoTextRenderMode();
// 	std::string debugLine=makeDebugLines();
	Vector2 textpos;
	float fontheight=15.f;
// 	float aspect=3.f/4.f;
	float pad=2.5f;
// 	bool isCentered=false;
	textpos.x=g_theWorldBounds->m_mins.x+pad;
	textpos.y=g_theWorldBounds->m_maxs.y-pad;
// 
// 	Vector2 shadowpos=textpos;
// 	float shadowDisplacementScale=2.5f;
// 	float shadowAlpha=0.5f;
// 	float bgAlpha=0.25f;
// 	Rgba textcolor=Rgba::WHITE;
// 	Rgba shadowcolor=Rgba::BLACK;
// 	Rgba bgcolor=Rgba::BLUE;
// 	shadowcolor.setAlphaAsFloat(shadowAlpha);
// 	bgcolor.setAlphaAsFloat(bgAlpha);
// 	shadowpos+=Vector2(1.f,-1.f)*shadowDisplacementScale;
	
// 	AABB2 bgboxBounds=*g_theWorldBounds;
// 	bgboxBounds.m_mins.y=bgboxBounds.m_maxs.y-14.f*fontheight;
// 	bgboxBounds.m_maxs.x=fontheight*aspect*45.f;
// 	g_theRenderer->drawFilledBox(bgboxBounds,Rgba(bgcolor));
// 	g_theRenderer->drawBox(bgboxBounds,textcolor);
// 	g_theRenderer->drawTextLines(g_theFont,debugLine,shadowpos,fontheight,aspect,shadowcolor,isCentered);
// 	g_theRenderer->drawTextLines(g_theFont,debugLine,textpos,fontheight,aspect,textcolor,isCentered);

	g_theDebugMessagePrinter->print(textpos,fontheight);

	goIntoSceneRenderMode();
	}

void Game::updateCamera()
	{
	//TODO: update camera here if needed.7
	}

Game::Game()
	: m_timescale(10)
	, m_isPaused(false)
	, m_isQuittingGame(false)
	, m_currentShakeAmount(0.f)
	, m_currentShakeTimeLeft(0.f)
	, m_cam()
	{
	init();
	}

void Game::init()
	{
	//g_tileAtlas=m_tileAtlas=g_theRenderer->createOrGetSpriteSheet(std::string("Data/Images/Test_StbiAndDirectX.png"),16,16);

	g_doShowDebugGraphics = true;

	doUnitTests();

	m_cam.m_fovVerticalDegrees=60.f;
	m_cam.m_aspectRatio=g_worldWidth/g_worldHeight;
	m_cam.m_nearDistance=0.01f;
	m_cam.m_farDistance=1000.f;

	}

Game::~Game()
	{

	}

void Game::update(float deltaSeconds)
	{
	if(g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->clear();
	if(g_theInput->isKeyDown('T'))
		{
		deltaSeconds*=0.1f;
		if(g_theDebugMessagePrinter)
			g_theDebugMessagePrinter->addMessage(std::string("Slow-Motion Mode(10% time scale)"),Rgba::LIME);
		}

	//float curtime=(float)GetCurrentTimeSeconds();

	updateScreenShakeTimer(deltaSeconds);

	updateInput(deltaSeconds);

	if (g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->addMessage(makeDebugLines(),Rgba::WHITE);

	updateCamera();
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
	g_theRenderer->clearScreenAndDepth(Rgba::BLACK);

	goIntoSceneRenderMode();

	g_theRenderer->drawAxes(1000.f);

	float lineThickness=1.f;
	Sphere3 sphere;

	sphere.m_center=Vector3::ZERO;
	sphere.m_radius=50.f;
	g_theRenderer->drawWireframeSphere3(sphere,Rgba::WHITE,lineThickness);

	if (g_doShowDebugGraphics)
		{
		drawDebugDots();
		displayGameDebugInfo();
		}

	displayHUD();
	}

void Game::toggleDebugGraphics()
	{
	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}