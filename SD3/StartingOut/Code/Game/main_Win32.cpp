#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// #include <gl/gl.h>					// #ToDo: Remove all references to OpenGL
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/File/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library

#include "Engine/Gui/Window.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/File/ConfigFileParser.hpp"

#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/RHI/RHIOutput.hpp"
#include "Engine/Renderer/RHI/RHIInstance.hpp"

//--------------------------------

App* g_theApp=nullptr;
Window* g_theWindow=nullptr;

//------------------------------------------------

RHIDeviceContext* g_context=nullptr;
RHIOutput* g_output=nullptr;

//-----------------------------------------------------------------------------------------------
/*#define UNUSED(x) (void)(x);*/

//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
	{
	unsigned char asKey = (unsigned char)wParam;
	switch (wmMessageCode)
		{
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
			g_theApp->setIsQuitting(true);
			return 0;

		case WM_KEYDOWN:
			if (asKey == VK_ESCAPE)
				{
				g_theApp->setIsQuitting(true);
				return 0;
				}

			g_theApp->registerKeyDown(asKey);

			break;

		case WM_KEYUP:
			g_theApp->registerKeyUp(asKey);

			break;
		}

	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
	}

ConfigSettings g_configSettings;


void configSystemStartup(std::string configFileName,std::string cmdLine)
	{
	ConfigFileParser parser;
	parser.parseFile(configFileName);
// 	parser.debugPrintSymbols();
	std::map<std::string,std::string> symbolTable;
	symbolTable=parser.getSymbolTable();

	g_configSettings.m_windowTitle=symbolTable.at(std::string("windowTitle"));
	g_configSettings.m_xres=stringToInt(symbolTable.at(std::string("windowResolutionX")));
	g_configSettings.m_yres=stringToInt(symbolTable.at(std::string("windowResolutionY")));
	g_configSettings.m_left=stringToInt(symbolTable.at(std::string("windowLeft")));
	g_configSettings.m_top=stringToInt(symbolTable.at(std::string("windowTop")));
	g_configSettings.m_shaderFile=symbolTable.at(std::string("shaderFile"));
	g_configSettings.m_imageFile=symbolTable.at(std::string("imageFile"));
	}

void configSystemShutdown()
	{
	
	}
 
void RHISystemStartup()
	{
	RHIInstance::getInstance()->createOutput(WindowsMessageHandlingProcedure,g_context,
											 g_output, g_configSettings.m_windowTitle,
											 g_configSettings.m_left,g_configSettings.m_top,g_configSettings.m_xres,g_configSettings.m_yres);
	}

void RHISystemShutdown()
	{
	//TODO: is there anything to do in this function?  can't delete a singleton with private destructor
	}

//-----------------------------------------------------------------------------------------------
void Initialize(HINSTANCE applicationInstanceHandle,std::string cmdLine)
	{
	//DebuggerPrintf("Hello World.\n");\
	//ERROR_RECOVERABLE("Well, that sucked.\n");
	
	
	configSystemStartup(std::string("config/startup.config"),cmdLine);
	RHISystemStartup();
// 	InputSystemStartup(); // Assume you have input systems from previous class

	g_theApp = new App();
	g_theApp->setMsgProc(WindowsMessageHandlingProcedure);
// 	g_theWindow=new Window(applicationInstanceHandle,WindowsMessageHandlingProcedure,g_configSettings.m_windowTitle,
// 						   g_configSettings.m_left,g_configSettings.m_top,g_configSettings.m_xres,g_configSettings.m_yres);
// 	g_theWindow->show();
// 	std::string cmdLineStr(cmdLine);

	}

//-----------------------------------------------------------------------------------------------
void Shutdown()
	{
// 	delete g_theWindow;
// 	g_theWindow=nullptr;
	delete g_theApp;
	g_theApp = nullptr;

//  inputSystemShutdown();
	RHISystemShutdown();
	configSystemShutdown();
	}

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd)
	{
	UNUSED(cmdLine);
	UNUSED(prevInstance);
	UNUSED(showCmd);

	// INITIALIZE ENGINE SYSTEMS NEEDED
	Initialize(instance,cmdLine);

	g_theApp->run();

	Shutdown();
	return 0;
	}

