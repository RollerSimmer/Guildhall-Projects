#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>					// #ToDo: Remove all references to OpenGL
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library

#include "Engine/Core/ErrorWarningAssert.hpp"
#include <stack>


//-----------------------------------------------------------------------------------------------
const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const int WINDOW_PHYSICAL_WIDTH = 1500;
const int WINDOW_PHYSICAL_HEIGHT = WINDOW_PHYSICAL_WIDTH*9/16;
const float g_worldWidth=(float)WINDOW_PHYSICAL_WIDTH;
const float g_worldHeight=(float)WINDOW_PHYSICAL_HEIGHT;
const double VIEW_LEFT = 0.0;
const double VIEW_RIGHT = g_worldWidth;
const double VIEW_BOTTOM = 0.0;
const double VIEW_TOP = g_worldHeight;
//const double VIEW_TOP = 0.0;


//-----------------------------------------------------------------------------------------------
App* g_theApp=nullptr;
bool g_isQuitting = false;
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "Simple Miner by John Anderson";


//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	unsigned char asKey = (unsigned char)wParam;
// 	unsigned short mouseflags=(unsigned short)wParam;
	struct MessageMouseCoords 
		{
		unsigned short x;
		unsigned short y;
		};
	MessageMouseCoords mousecoords=*(MessageMouseCoords*)&lParam;
	IntVector2 intVec2MouseCoords=IntVector2(mousecoords.x,mousecoords.y);

	static bool doesHaveFocus=false;
	static bool mousePosInit=false;
	switch (wmMessageCode)
		{
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
			if (g_theApp)
				g_theApp->setIsQuitting(true);
			return 0;

		case WM_KEYDOWN:
			if (asKey == KEYCODE_ESCAPE)
				{
				if (g_theApp)
					g_theApp->setIsQuitting(true);
				return 0;
				}

			if (g_theApp)
				g_theApp->registerKeyDown(asKey);

			break;
		case WM_SETFOCUS:
			if (g_theApp)
				g_theApp->onGainedFocus();

			break;

		case WM_KILLFOCUS:
			if (g_theApp)
				g_theApp->onLostFocus();

			break;

		case WM_KEYUP:
			if (g_theApp)
				g_theApp->registerKeyUp(asKey);

			break;
// 		case WM_MOUSEMOVE:
// 			if(mousePosInit)
// 				g_theApp->registerMouseMove(mouseflags,intVec2MouseCoords);
// 			else
// 				{
// 				g_theApp->initMousePosition(mouseflags,intVec2MouseCoords);
// 				mousePosInit=true;
// 				}
// 			break;
		case WM_LBUTTONDOWN:
			if (g_theApp)
				g_theApp->registerKeyDown(KEYCODE_LMOUSEBUTTON);
			break;
		case WM_LBUTTONUP:
			if (g_theApp)
				g_theApp->registerKeyUp(KEYCODE_LMOUSEBUTTON);
			break;
		case WM_RBUTTONDOWN:
			if (g_theApp)
				g_theApp->registerKeyDown(KEYCODE_RMOUSEBUTTON);
			break;
		case WM_RBUTTONUP:
			if (g_theApp)
				g_theApp->registerKeyUp(KEYCODE_RMOUSEBUTTON);
			break;
		}

	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}


//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow(HINSTANCE applicationInstanceHandle)
{
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);

	// get desktop width and height
	int desktopWidth=desktopRect.right-desktopRect.left;
	int desktopHeight=desktopRect.bottom-desktopRect.top;

	// set window width and height to a percentage of the desktop width and height
	const float PCT_OF_DESKTOP_SIZE_USED = 0.85f;
	int windowHeight=(int)((float)desktopHeight*PCT_OF_DESKTOP_SIZE_USED);
	int windowWidth=windowHeight*16/9;

	// set bounds for window
	int windowOffsetX = (desktopWidth-windowWidth)/2;
	int windowOffsetY = (desktopHeight-windowHeight)/2;

	int left = windowOffsetX;
	int top = windowOffsetY;
	int right = left+windowWidth;
	int bottom = top+windowHeight;

	RECT windowRect = { left, top, right, bottom };
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL);
	

	g_displayDeviceContext = GetDC(g_hWnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
	pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	int pixelFormatCode = ChoosePixelFormat(g_displayDeviceContext, &pixelFormatDescriptor);
	SetPixelFormat(g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor);
	g_openGLRenderingContext = wglCreateContext(g_displayDeviceContext);
	wglMakeCurrent(g_displayDeviceContext, g_openGLRenderingContext);

	g_theRenderer->enable(GL_BLEND);
	g_theRenderer->blendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	g_theRenderer->setLineWidth(2.f);
	g_theRenderer->enable(GL_LINE_SMOOTH);
}


//-----------------------------------------------------------------------------------------------
// Processes all windows messages (WM_xxx) for this app that have queued up since last frame.
//
void RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}

//-----------------------------------------------------------------------------------------------
void Initialize(HINSTANCE applicationInstanceHandle)
{
	//DebuggerPrintf("Hello World.\n");\
	//ERROR_RECOVERABLE("Well, that sucked.\n");
	SetProcessDPIAware();
	CreateOpenGLWindow(applicationInstanceHandle);
	g_theApp = new App();

	ShowWindow(g_hWnd,SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);
}

//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	delete g_theApp;
	g_theApp = nullptr;
}

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
	{
	UNUSED(commandLineString);
	Initialize(applicationInstanceHandle);

	while (!g_theApp->isQuitting())
		{
		Sleep(0);
		RunMessagePump();
		g_theApp->runFrame();
		SwapBuffers(g_displayDeviceContext);
		}

	Shutdown();
	return 0;
	}
