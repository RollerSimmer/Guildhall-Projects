#define WIN32_LEAN_AND_MEAN
#include "Game/App.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/InputSys.hpp"
#include "Engine/Renderer/RHI/RHIInstance.hpp"
#include "Engine/Renderer/RHI/RHIOutput.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Gui/Window.hpp"
#include "Engine/Renderer/RgbaFloat.hpp"
#include "Engine/Renderer/Vertex3.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include "Engine/Renderer/RHI/Sampler.hpp"
#include "Engine/Renderer/RHI/ConstantBuffer.hpp"

//TODO: parse font definition files
//TODO: implement all font methods
//TODO: get basics of assignment 3 completed

InputSys* g_theInput=nullptr;
Renderer* g_theRenderer=nullptr;
AABB2* g_theWorldBounds=nullptr;

bool g_doShowDebugGraphics=false;

int g_amtVerticesToDraw=0;

const float g_worldWidth=(float)DEFAULT_WINDOW_WIDTH;
const float g_worldHeight=(float)DEFAULT_WINDOW_HEIGHT;

App::App()
	: m_game(nullptr)
	, m_isQuitting(false)
	, m_pauseState(false)
	, m_slowmoState(false)
	, m_pauseFactor(1.f)
// 	,m_rhiDevice(nullptr)
// 	,m_rhiContext(nullptr)
// 	,m_rhiOutput(nullptr)
	,m_texSample(nullptr)
	{

	}

App::~App()
	{
	}

void App::start()
	{
	g_theInput = new InputSys();
	float pad=0.f;
	g_theWorldBounds=new AABB2(pad,pad,g_worldWidth-pad,g_worldHeight-pad);
	m_game = new Game();
	std::string windowTitle=g_configSettings.m_windowTitle;
	initRendering();
	//g_theRenderer=new Renderer();

	}

void App::end()
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
void App::update(float deltaSeconds)
	{
	if(m_renderer->m_rhiOutput)
		m_renderer->m_rhiOutput->m_window->processMessages();
	if (g_theInput)
		g_theInput->update(deltaSeconds);
	if(m_game)
		m_game->update(deltaSeconds);


	if (g_theInput->isKeyDown(KEYCODE_ESCAPE) || g_theInput->getXboxController(XCONTROLLER_0).isXBoxButtonPressed(XBUTTON_BACK))
		this->setIsQuitting(true);

	const float zoomFactor=1.1f;
	const float invZoomFactor=1.f/zoomFactor;
	if (g_theInput->isKeyDown('W'))
		this->m_gameState.m_zoom*=zoomFactor;
	if (g_theInput->isKeyDown('S'))
		this->m_gameState.m_zoom*=invZoomFactor;
	
	this->m_gameState.m_time+=deltaSeconds;

	if(m_renderer->m_rhiOutput->m_window->isClosed())
		this->setIsQuitting(true);

// 	m_gameStateConstants->update( m_renderer->m_rhiContext, &m_gameState );
	}
	
//-----------------------------------------------------------------------------------------------
// Processes all windows messages (WM_xxx) for this app that have queued up since last frame.
//
void App::runMessagePump()
	{
	MSG queuedMessage;
	for (;; )
		{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage,NULL,0,0,PM_REMOVE);
		if (!wasMessagePresent)
			{
			break;
			}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
		}
	}

//-----------------------------------------------------------------------------------------------
void App::runFrame()
	{
	beginFrame();


	static double timeOfLastRunFrame=GetCurrentTimeSeconds();
	double timeNow = GetCurrentTimeSeconds();
	double deltaSeconds=timeNow-timeOfLastRunFrame;
	timeOfLastRunFrame=timeNow;
	m_timeConstants->update(m_renderer->m_rhiContext,&m_time);

	DebuggerPrintf("time: %f; time before: %f; deltaSeconds: %f",(float)timeOfLastRunFrame,(float)timeNow,(float)deltaSeconds);

	update((float) deltaSeconds);
	render();
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


void App::initRendering()
	{
	int left=g_configSettings.m_left;
	int top=g_configSettings.m_top;
	int width=g_configSettings.m_xres;
	int height=g_configSettings.m_yres;

// 	m_rhiContext=g_context;
	m_renderer=new SimpleRendererDx();

	m_renderer->setup(m_msgProc, g_configSettings.m_windowTitle,left,top,width,height);

	m_myShader=m_renderer->m_rhiDevice->createShaderFromFile(g_configSettings.m_shaderFile.c_str());

	int vertSize=sizeof(Vertex3F);
	// Create vertices
	float d=1.f;
	float xs = -200.f;
	float ys = 200.f;

	RgbaFloat color(1.f,1.f,0.f,1.f);

	Vertex3F vertices[] = {
		Vertex3F(Vector3(-xs, ys, 0.f) ,color,Vector2(0.f,0.f)),
		Vertex3F(Vector3(xs, ys, 0.f)  ,color,Vector2(d,0.f)),
		Vertex3F(Vector3(-xs, -ys, 0.f),color,Vector2(0.f,d)),
		Vertex3F(Vector3(-xs, -ys, 0.f),color,Vector2(0.f,d)),
		Vertex3F(Vector3(xs, ys, 0.f)  ,color,Vector2(d,0.f)),
		Vertex3F(Vector3(xs, -ys, 0.f) ,color,Vector2(d,d)),
		};
	g_amtVerticesToDraw=sizeof(vertices)/sizeof(vertices[0]);

	m_quadVbo = m_renderer->m_rhiDevice->createVertexBuffer(vertices,g_amtVerticesToDraw);

	m_pointSampler = new Sampler(m_renderer->m_rhiDevice, FILTER_POINT, FILTER_POINT);
	m_texSample = new RHITexture2D(m_renderer->m_rhiDevice, g_configSettings.m_imageFile.c_str());
	m_texFontSheet=new RHITexture2D(m_renderer->m_rhiDevice,"font/linearFonts.png");
	
	m_time.m_time=0.f;
	m_timeConstants = new ConstantBuffer(m_renderer->m_rhiDevice,&m_time,sizeof(m_time));
	}

void App::run()
	{
	start();
	while (!isQuitting())
		{
		Sleep(1);
		runMessagePump();
		runFrame();
/*		break;*/
		}
	end();
	}

//-----------------------------------------------------------------------------------------------
// This function will be removed once we add in an Application class
//
void App::render() const
	{
	static int x=0;
// 	static int counter=0;
// 	counter++;
// 	if(counter>10)
// 		x++;
// 	Rgba clearColor=Rgba::calcNoiseColor(x);
	Rgba clearColor=Rgba::BLUE;

	RgbaFloat clearColorF(clearColor);
	m_renderer->setRenderTarget(nullptr);
	m_renderer->clearTargetColor(clearColorF);

	m_renderer->setViewport(0,0,g_configSettings.m_xres,g_configSettings.m_yres);

	m_renderer->setShader(m_myShader);
	m_renderer->setTexture(m_texSample);
	m_renderer->setSampler(m_pointSampler);

	float d = 200.f;

	m_renderer->setOrthoProjection(Vector2(-d,-d),Vector2(d,d));
// 	m_renderer->setConstantBuffer(1,m_gameStateConstants);

	m_renderer->draw(DX_PRIMITIVE_TRIANGLES,m_quadVbo,g_amtVerticesToDraw);

	std::string name="All bosses considered, deadly enemies fight gruelingly hard in journeys known less maddeningly nasty on potion quests rightly situated towns under vast xylophone's yearly zest";
	float tx=-200.f;
	float ty= 200.f;
	float s = 100.f;
	float size=16;
	float charWidth,charHeight;
	charWidth=charHeight=10.f;
	ty-=charHeight;
	for (int i=0;i<name.length();i++)
		{
		char c = name[i];
		int glyphIndex=c;
		int row = glyphIndex/16;
		int col = glyphIndex-(row*16);
		float unit=1.f/16.f;
	
		Vector2 uvMins((float)col*unit,      (float)row*unit);
		Vector2 uvMaxs((float)(col+1)*unit,  (float)(row+1)*unit);

		m_renderer->setTexture(m_texFontSheet);
		m_renderer->enableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
		m_renderer->drawQuad2D(Vector2(tx,ty),Vector2(tx+charWidth,ty+charHeight),uvMins,uvMaxs,RgbaFloat(1.f,1.f,0.f,0.1f));
		m_renderer->disableBlend();

		tx+=charWidth;
		}

	m_renderer->present();
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

