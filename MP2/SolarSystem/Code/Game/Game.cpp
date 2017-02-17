#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/CelestialNode.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/File/FileUtils.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Game/SceneNode.hpp"
#include "Game/CelestialNode.hpp"
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

Game::Game()
	: m_timescale(10)
	,m_isPaused(false)
	,m_isQuittingGame(false)
	,m_currentShakeAmount(0.f)
	,m_currentShakeTimeLeft(0.f)
	,m_cam()
	,m_solarSystem(nullptr)
	,m_targetNode(nullptr)
	,m_targetIndex(0)
	,m_focusNode(nullptr)
	,m_focusIndex(0)
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
	m_cam.m_farDistance=10000000.f;


	initSolarSystem();
	}

Game::~Game()
	{

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

void Game::addDebugDotAt(const Vector3& pos)
	{
	m_debugDotPoints.push_back(pos);
	}

void Game::initSolarSystem()
	{
//	const float AU=10.f;

	float orbitDistanceScale=0.5f;

	m_nodeList.resize(0);
	m_nodeList.push_back(nullptr);

	float orbitSpeedMultiplier=10.f;
	float planetScale=5.f;
	float moonScale=7.f;
	float starScale=1.f;

	std::string name="Sun";
	CelestialNode* parent=nullptr;
	Rgba color=Rgba::YELLOW;
	float angularVel=45.f;
	Vector3 relPos(0.f,0.f,0.f);
	Vector3 rotAxis(0.f,0.f,1.f);
	float radius=100.f;
	radius*=starScale;
	float orbitAngularVel=0.f*orbitSpeedMultiplier;
	Vector3 orbitalCenter(Vector3::ZERO);
	Vector3 orbitalPlaneX(Vector3::X_AXIS_VECTOR);
	Vector3 orbitalPlaneY(Vector3::Y_AXIS_VECTOR);
	Vector2 ijDistances(0.f,0.f);
	ijDistances*=orbitDistanceScale;
	CelestialNode* sun=new CelestialNode(parent,name,relPos,rotAxis,color,angularVel,radius,orbitAngularVel,orbitalCenter,
										 orbitalPlaneX,orbitalPlaneY,ijDistances);
	m_nodeList.push_back(sun);

	name="Earth";
	parent=sun;
	color=Rgba::GREEN;
	angularVel=20.f;
	rotAxis=Vector3(0.f,0.f,1.f);
	radius=2.f;
	radius*=planetScale;
	orbitAngularVel=1.f*orbitSpeedMultiplier;
	orbitalCenter=Vector3(0.f,10.f,0.f);
	orbitalPlaneX=Vector3(1.f,0.f,0.f);
	orbitalPlaneY=Vector3(0.f,1.f,0.f);
	ijDistances=Vector2(450.f,425.f);
	ijDistances*=orbitDistanceScale;
	CelestialNode* earth=new CelestialNode(parent,name,relPos,rotAxis,color,angularVel,radius,orbitAngularVel,orbitalCenter,
										   orbitalPlaneX,orbitalPlaneY,ijDistances);
	sun->addChild(earth);
	m_nodeList.push_back(earth);

	name="Moon";
	parent=earth;
	color=Rgba::GRAY;
	angularVel=1.f;
	rotAxis=Vector3(0.f,0.f,1.f);
	radius=0.5f;
	radius*=moonScale;
	orbitAngularVel=12.f*orbitSpeedMultiplier;
	orbitalCenter=Vector3(0.f,0.f,0.f);
	orbitalPlaneX=Vector3(0.f,1.f,0.f);
	orbitalPlaneY=Vector3(0.f,0.f,1.f);
	ijDistances=Vector2(50.f,50.f);
	ijDistances*=orbitDistanceScale;
	CelestialNode* moon=new CelestialNode(parent,name,relPos,rotAxis,color,angularVel,radius,orbitAngularVel,orbitalCenter,
										   orbitalPlaneX,orbitalPlaneY,ijDistances);
	earth->addChild(moon);
	m_nodeList.push_back(moon);

	name="Mars";
	parent=sun;
	color=Rgba::RED;
	angularVel=10.f;
	rotAxis=Vector3(0.f,1.f,0.f);
	radius=0.7f;
	radius*=planetScale;
	orbitAngularVel=0.5f*orbitSpeedMultiplier;
	orbitalCenter=Vector3(0.f,0.f,0.f);
	orbitalPlaneX=Vector3(1.f,0.f,0.f);
	orbitalPlaneY=Vector3(0.f,1.f,0.f);
	ijDistances=Vector2(600.f,600.f);
	ijDistances*=orbitDistanceScale;
	CelestialNode* mars=new CelestialNode(parent,name,relPos,rotAxis,color,angularVel,radius,orbitAngularVel,orbitalCenter,
											orbitalPlaneX,orbitalPlaneY,ijDistances);
	sun->addChild(mars);
	m_nodeList.push_back(mars);

	name="Phobos";
	parent=mars;
	color=Rgba(0x8f7f9fff);
	angularVel=1.f;
	rotAxis=Vector3(0.f,0.f,1.f);
	radius=0.1f;
	radius*=moonScale;
	orbitAngularVel=12.f*orbitSpeedMultiplier;
	orbitalCenter=Vector3(0.f,0.f,0.f);
	orbitalPlaneX=Vector3(1.f,0.f,0.f);
	orbitalPlaneY=Vector3(0.f,1.f,0.f);
	ijDistances=Vector2(15.f,15.f);
	ijDistances*=orbitDistanceScale;
	CelestialNode* phobos=new CelestialNode(parent,name,relPos,rotAxis,color,angularVel,radius,orbitAngularVel,orbitalCenter,
										  orbitalPlaneX,orbitalPlaneY,ijDistances);
	mars->addChild(phobos);
	m_nodeList.push_back(phobos);

	name="Jupiter";
	parent=sun;
	color=Rgba::ORANGE;
	angularVel=20.f;
	rotAxis=Vector3(0.1f,0.f,1.f);
	radius=10.f;
	radius*=planetScale;
	orbitAngularVel=0.5f*orbitSpeedMultiplier;
	orbitalCenter=Vector3(0.f,0.f,0.f);
	orbitalPlaneX=Vector3(1.f,1.f,1.f);
	orbitalPlaneY=Vector3(1.f,-1.f,1.f);
	ijDistances=Vector2(-1000.f,1200.f);
	ijDistances*=orbitDistanceScale;
	CelestialNode* jupiter=new CelestialNode(parent,name,relPos,rotAxis,color,angularVel,radius,orbitAngularVel,orbitalCenter,
											 orbitalPlaneX,orbitalPlaneY,ijDistances);
	sun->addChild(jupiter);
	m_nodeList.push_back(jupiter);

	name="Iapetus";
	parent=jupiter;
	color=Rgba(0x8f7f6f);
	angularVel=1.f;
	rotAxis=Vector3(0.f,0.f,1.f);
	radius=0.5f;
	radius*=moonScale;
	orbitAngularVel=12.f*orbitSpeedMultiplier;
	orbitalCenter=Vector3(0.f,0.f,0.f);
	orbitalPlaneX=Vector3(0.f,1.f,0.f);
	orbitalPlaneY=Vector3(0.f,0.f,1.f);
	ijDistances=Vector2(200.f,200.f);
	ijDistances*=orbitDistanceScale;
	CelestialNode* iapetus=new CelestialNode(parent,name,relPos,rotAxis,color,angularVel,radius,orbitAngularVel,orbitalCenter,
										  orbitalPlaneX,orbitalPlaneY,ijDistances);
	jupiter->addChild(iapetus);
	m_nodeList.push_back(iapetus);

	name="Uranus";
	parent=sun;
	color=Rgba::CYAN;
	angularVel=10.f;
	rotAxis=Vector3(0.f,1.f,0.f);
	radius=7.f;
	radius*=planetScale;
	orbitAngularVel=0.25f*orbitSpeedMultiplier;
	orbitalCenter=Vector3(0.f,0.f,0.f);
	orbitalPlaneX=Vector3(1.f,0.f,0.f);
	orbitalPlaneY=Vector3(0.f,1.f,0.f);
	ijDistances=Vector2(-2000.f,2000.f);
	ijDistances*=orbitDistanceScale;
	CelestialNode* uranus=new CelestialNode(parent,name,relPos,rotAxis,color,angularVel,radius,orbitAngularVel,orbitalCenter,
											orbitalPlaneX,orbitalPlaneY,ijDistances);
	sun->addChild(uranus);
	m_nodeList.push_back(uranus);

	m_solarSystem.setRoot(sun);
	}

void Game::jumpToNextPlanet()
	{
	m_targetIndex++;
	if(m_targetIndex>=(int)m_nodeList.size())
		m_targetIndex=0;
	m_targetNode=m_nodeList.at(m_targetIndex);
	}

void Game::cycleFocusPlanet()
	{
	m_focusIndex++;
	if (m_focusIndex>=(int)m_nodeList.size())
		m_focusIndex=0;
	m_targetNode=m_nodeList.at(m_targetIndex);
	}

void Game::lockCameraPositionToPlanet()
	{
	if(m_targetNode==nullptr)
		return;
	while(m_targetNode->getName()=="Earth")
		break;
	Vector3 targetPos=m_targetNode->calcAbsolutePosition();
	Vector3 disp=m_cam.m_pos-targetPos;
	if(disp.calcLengthSquared()==0.f)
		disp=Vector3::Z_AXIS_VECTOR;
	float distFromCenter=m_targetNode->getRadius()*1.5f;
	disp.setLength(distFromCenter);
	m_cam.m_pos=targetPos+disp;
	if(g_theDebugMessagePrinter)
		{
		g_theDebugMessagePrinter->addMessage(disp.asString(),Rgba::MAGENTA);
		g_theDebugMessagePrinter->addMessage(m_cam.m_pos.asString(),Rgba::CYAN);
		g_theDebugMessagePrinter->addMessage(targetPos.asString(),Rgba::LIME);
		}
	}

bool Game::lookAtFocus()
{
	if (m_targetNode==nullptr || m_focusNode==nullptr)
		return false;
	Vector3 fromPos=m_targetNode->calcAbsolutePosition();
	Vector3 up=Vector3::Z_AXIS_VECTOR;
	Vector3 toPos=m_focusNode->calcAbsolutePosition();

	Vector3 disp=toPos-fromPos;
	disp.setLength(m_targetNode->getRadius());
	fromPos+=disp;

	g_theRenderer->lookAt(fromPos,toPos,up);
// 	m_cam.m_pos=fromPos;
// 	Matrix4 viewMatrix=g_theRenderer->getCurrentModelViewMatrix();
// 	m_cam.m_yawDegreesAboutZ=atan2Degrees(viewMatrix.getCell(2,0),viewMatrix.getCell(0,0));
// 	m_cam.m_pitchDegreesAboutY=asinfDegrees(viewMatrix.getCell(1,2));
// 	m_cam.m_rollDegreesAboutX=atan2Degrees(viewMatrix.getCell(1,2),viewMatrix.getCell(1,1));
	
	std::stringstream ss;
	ss.str("");
	ss<<"You are currently looking at "<<(m_focusNode==nullptr?"nothing":m_focusNode->getName())<<" from "<<m_targetNode->getName();
	if(g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->addMessage(ss.str(),Rgba::WHITE);
	
	return m_focusNode!=nullptr;
	}

void Game::updateCurrentFocusBodyMessage()
	{
	if(g_theDebugMessagePrinter==nullptr)
		return;
	std::stringstream ss;
	ss.str("");
	ss<<"The current celestial body is \"";
	Rgba msgColor;
	if (m_targetNode)
		{
		ss<<m_targetNode->getName();
		msgColor=m_targetNode->getColor();
		}
	else
		{
		ss<<"FREE CAM";
		msgColor=Rgba::WHITE;
		}
	ss<<"\"";
	g_theDebugMessagePrinter->addMessage(ss.str(),msgColor);
	}

void Game::doUnitTests()
	{
	
	}

void Game::displayHUD() 
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

void Game::doScreenShake()
	{
	float screenShakeX = GetRandomFloatInRange(-m_currentShakeAmount, m_currentShakeAmount);
	float screenShakeY = GetRandomFloatInRange(-m_currentShakeAmount, m_currentShakeAmount);
	g_theRenderer->translate2D(Vector2(screenShakeX, screenShakeY));
	}

void Game::goIntoSceneRenderMode() 
	{

	g_theRenderer->setPerspectiveProjection(m_cam.m_fovVerticalDegrees,
											m_cam.m_aspectRatio,m_cam.m_nearDistance,
											m_cam.m_farDistance);

	
	if (m_focusNode!=m_targetNode)
		{
		lookAtFocus();
		}
	else
		{
		g_theRenderer->rotate3D(-90.f,Vector3::X_AXIS_VECTOR);
		g_theRenderer->rotate3D(90.f,Vector3::Z_AXIS_VECTOR);

		g_theRenderer->rotate3D(-m_cam.m_rollDegreesAboutX,Vector3::X_AXIS_VECTOR);
		g_theRenderer->rotate3D(-m_cam.m_pitchDegreesAboutY,Vector3::Y_AXIS_VECTOR);
		g_theRenderer->rotate3D(-m_cam.m_yawDegreesAboutZ,Vector3::Z_AXIS_VECTOR);

		g_theRenderer->translate3D(-m_cam.m_pos);

		}

	g_theRenderer->enableBackFaceCulling(true);
	g_theRenderer->enableDepthTesting(true);
	}

void Game::goIntoTextRenderMode() 
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

std::string Game::makeDebugLines() 
	{
	std::stringstream ss;
	ss.str("");
	//ss<<"Mouse Displacement: "<<g_theInput->calcMouseMoveDisplacement().asString()<<std::endl;
	ss<<"Camera: "<<m_cam.asString()<<std::endl;

	ss<<"Camera mode: "<<m_cam.getModeName()<<std::endl;
	ss<<"W,S moves camera forward and backward" <<std::endl;
	ss<<"A,D moves camera left and right"       <<std::endl;
	ss<<"F5 switches home planet/star"          <<std::endl;
	ss<<"F6 switches focus planet/star"         <<std::endl;
	ss<<"T slows down time"						<<std::endl;
	ss<<"R speeds up time"						<<std::endl;
	ss<<"ESC leaves the application"            <<std::endl;
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

	if (g_theInput->wasKeyJustPressed(KEYCODE_F6))
		{
		++m_focusIndex;
		if(m_focusIndex>=m_nodeList.size())
			m_focusIndex=0;
		m_focusNode=m_nodeList.at(m_focusIndex);
		}
	}

void Game::updatePositionInput(float deltaSeconds)
	{
	float moveAmt=deltaSeconds*MOVE_SPEED;
	Vector3 moveDisplacement=Vector3::ZERO;
	Vector3 forward=m_cam.getForward();
	Vector3 left=m_cam.getLeftXY();
	Vector3 up=/*m_cam.m_mode==CAMERA::NO_CLIP ? */Vector3::Z_AXIS_VECTOR/* : Vector3::ZERO*/;
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
	if (g_theInput->isKeyDown(KEYCODE_SHIFT)/* && m_cam.m_mode==CAMERA::NO_CLIP*/)
		moveAmt*=100.f;
	// 	if(moveDisplacement==Vector3::ZERO)
	// 		return;

	moveDisplacement*=moveAmt;
	m_cam.m_pos+=moveDisplacement;

	if (g_theInput->wasKeyJustPressed(KEYCODE_F5))
		jumpToNextPlanet();
	}

void Game::updateCameraFromMouse()
	{
	if (!g_theInput->doesHaveFocus())
		return;
// 	if (m_cam.m_mode!=CAMERA::NO_CLIP)
// 		return;
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

void Game::drawDebugDots() 
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

void Game::displayGameDebugInfo() 
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
	g_theDebugMessagePrinter->setAllForegroundAlphasTo(0.75f);
	g_theDebugMessagePrinter->print(textpos,fontheight);

	goIntoSceneRenderMode();
	}

void Game::updateCamera()
	{
	//TODO: update camera here if needed.7
	}


void Game::update(float deltaSeconds)
	{
	if(g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->clear();
	if (g_theInput->isKeyDown('T'))
		{
		deltaSeconds*=0.1f;
		if (g_theDebugMessagePrinter)
			g_theDebugMessagePrinter->addMessage(std::string("Slow-Motion Mode(10% time scale)"),Rgba::LIME);
		}
	else if (g_theInput->isKeyDown('R'))
		{
		deltaSeconds*=10.f;
		if (g_theDebugMessagePrinter)
			g_theDebugMessagePrinter->addMessage(std::string("Fast-Motion(1000% time scale)"),Rgba::RED);
		}

	//float curtime=(float)GetCurrentTimeSeconds();

	updateScreenShakeTimer(deltaSeconds);
	
	updateInput(deltaSeconds);
	updateCurrentFocusBodyMessage();

	lockCameraPositionToPlanet();
	if (g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->addMessage(makeDebugLines(),Rgba::WHITE);

	m_solarSystem.update(deltaSeconds);

	updateCamera();
	}

void Game::updateScreenShakeTimer(float deltaSeconds)
	{
	if (m_currentShakeTimeLeft > 0.f)
		m_currentShakeTimeLeft -= (m_currentShakeTimeLeft > deltaSeconds ? deltaSeconds : m_currentShakeTimeLeft);
	if (m_currentShakeTimeLeft == 0.f)
		m_currentShakeAmount = 0.f;
	}

void Game::render()
	{
	g_theRenderer->clearScreenAndDepth(Rgba::BLACK);

	goIntoSceneRenderMode();

	g_theRenderer->drawAxes(1000.f);

// 	float lineThickness=1.f;
// 	Sphere3 sphere;

// 	sphere.m_center=Vector3::ZERO;
// 	sphere.m_radius=50.f;
// 	g_theRenderer->drawWireframeSphere3(sphere,Rgba::YELLOW,lineThickness);


	m_solarSystem.render();

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