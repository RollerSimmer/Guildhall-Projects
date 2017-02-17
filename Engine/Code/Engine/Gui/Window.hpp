#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

class IntVector2;
struct IntRect;

typedef  LRESULT (CALLBACK* WindowMessageHandlerCallback)(HWND,UINT,WPARAM,LPARAM);

class Window
	{
	public:
		Window(HINSTANCE applicationInstanceHandle,WindowMessageHandlerCallback msgCallback,std::string title, int left, int top,int xres,int yres );
		~Window();
		void open();
		void close();
		void create();
		void show();
		void hide();
		bool isOpen();
		void processMessages();
		void registerCustomMessageHandler(WindowMessageHandlerCallback msgCallback);
		IntRect getClientRect();
		void setTitle(const std::string& newWindowTitle);
		HWND getHandle() {return m_windowHandle;}
		int getWidth() {return m_xres;}
		int getHeight() {return m_yres;}
		bool isClosed();

	private:
		HINSTANCE m_applicationInstanceHandle;
		HWND m_windowHandle;
		HDC m_displayDeviceContext;
		WindowMessageHandlerCallback m_msgCallback;
		std::string m_title;
		int m_xres;
		int m_yres;
		int m_left;
		int m_top;
/*		HGLRC*/
	};
