#include "Engine/Gui/Window.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntRect.hpp"
#include "Engine/Renderer/Renderer.hpp"


const char* APP_NAME = "Protogame Console by John Anderson";

Window::Window(HINSTANCE applicationInstanceHandle,WindowMessageHandlerCallback msgCallback,std::string title, int left, int top,int xres,int yres) : m_applicationInstanceHandle(applicationInstanceHandle)
	,m_windowHandle(nullptr)
	,m_msgCallback(msgCallback)
	,m_xres(xres)
	,m_yres(yres)
	,m_title(title)
	,m_left(left)
	,m_top(top)
	{
	create();
	}

Window::~Window()
	{
	close();
	}

void Window::open()
	{
	show();
	}

bool Window::isOpen()
	{
	return m_windowHandle!=0;
	}

void Window::close()
	{
	//TODO: close the window
	DestroyWindow(m_windowHandle);
	m_windowHandle=0;
	}

void Window::processMessages()
	{
	
	}

void Window::registerCustomMessageHandler(WindowMessageHandlerCallback msgCallback)
	{
	m_msgCallback=msgCallback;
	}

IntRect Window::getClientRect()
	{
	RECT windowRect;
	GetClientRect(m_windowHandle,&windowRect);
	IntRect result(windowRect.left,windowRect.top,windowRect.right,windowRect.bottom);
	return result;
	}

void Window::setTitle(const std::string& newWindowTitle)
	{
	m_title=newWindowTitle;
	SetWindowTextA(m_windowHandle,m_title.c_str());
	}

bool Window::isClosed()
	{
	return !IsWindowVisible(m_windowHandle);
	}

void Window::create()
	{
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription,0,sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(m_msgCallback); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle,&desktopRect);

	RECT windowRect ={m_left, m_top, m_left + m_xres, m_top + m_yres};
	AdjustWindowRectEx(&windowRect,windowStyleFlags,FALSE,windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(),0,m_title.c_str(),-1,windowTitle,sizeof(windowTitle) / sizeof(windowTitle[0]));
	m_windowHandle = CreateWindowEx(
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
		m_applicationInstanceHandle,
		NULL);

	}

void Window::show()
	{
	ShowWindow(m_windowHandle,SW_SHOW);
	SetForegroundWindow(m_windowHandle);
	SetFocus(m_windowHandle);

	m_displayDeviceContext = GetDC(m_windowHandle);

	HCURSOR cursor = LoadCursor(NULL,IDC_ARROW);
	SetCursor(cursor);
	}

void Window::hide()
	{
	ShowWindow(m_windowHandle,SW_HIDE);
	}
