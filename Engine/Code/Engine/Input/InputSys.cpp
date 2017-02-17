#include "Engine/Input/InputSys.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

InputSys::InputSys()
	: m_doesHaveFocus(false)	
	{
	for (int ikey = 0; ikey < AMT_KEYBOARD_KEYS; ikey++)
		{
		m_keystates[ikey].m_isDown=false;
		m_keystates[ikey].m_justChanged=false;
		}
	for (int icontroller = XCONTROLLER_0; icontroller < AMT_XBOX_CONTROLLERS; icontroller++)
		{
		m_xboxControllers[icontroller]=new XboxController((XboxControllerNumber)icontroller);
		}

	centerCursorPos();
	}

InputSys::~InputSys()
	{
	for (int icontroller = XCONTROLLER_0; icontroller < AMT_XBOX_CONTROLLERS; icontroller++)
		{
		XboxControllerNumber ccontroller= (XboxControllerNumber)icontroller;
		delete m_xboxControllers[ccontroller];
		m_xboxControllers[ccontroller]=nullptr;
		}
	}

void InputSys::beginFrame()
	{
	//TODO: update xbox controller states
	}

void InputSys::endFrame()
	{
 	for (int ikey = 0; ikey < AMT_KEYBOARD_KEYS; ikey++)
		{
		KeyState& keystate = m_keystates[ikey];
// 		keystate.m_isDown = false;
		keystate.m_justChanged = false;
		}
	}

void InputSys::onGainedFocus()
	{
	m_doesHaveFocus=true;
	showMouseCursor(false);
	centerCursorPos();
	}

void InputSys::onLostFocus()
	{
	m_doesHaveFocus=false;
	showMouseCursor(true);
	}

IntVector2 InputSys::getCursorPos() const
{
	POINT point;
	GetCursorPos(&point);
	IntVector2 result(point.x,point.y);
	return result;
	}

IntVector2 InputSys::getWindowCenter()
	{
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle,&desktopRect);
	IntVector2 center((desktopRect.left+desktopRect.right)/2,(desktopRect.top+desktopRect.bottom)/2);
	return center;
	}

void InputSys::centerCursorPos()
	{
	IntVector2 center=getWindowCenter();
	setCursorPos(center);
	}

void InputSys::setCursorPos(const IntVector2& newpos)
	{
	SetCursorPos(newpos.x,newpos.y);
	}

void InputSys::registerKeyDown(unsigned char ikey)
	{
	KeyState& keystate = m_keystates[ikey];
	if(!keystate.m_isDown)
		keystate.m_justChanged=true;
	keystate.m_isDown=true;
	}

void InputSys::registerKeyUp(unsigned char ikey)
	{
	KeyState& keystate = m_keystates[ikey];
	if (keystate.m_isDown)
		keystate.m_justChanged = true;
	keystate.m_isDown = false;
	}

bool InputSys::isKeyDown(unsigned char ikey)
	{
	if (ikey < 0 || ikey > AMT_KEYBOARD_KEYS)
		return false;
	return m_keystates[ikey].m_isDown;
	}

bool InputSys::wasKeyJustPressed(unsigned char ikey)
	{
	KeyState& keystate = m_keystates[ikey];

	return keystate.m_isDown&&keystate.m_justChanged;
	}

void InputSys::update(float deltaSeconds)
	{
	// stub function
	// do something with deltaSeconds to suppress warning
	float f;
	f=deltaSeconds;
	m_mouseState.update(deltaSeconds);
	for (int icontroller = XCONTROLLER_0; icontroller < AMT_XBOX_CONTROLLERS; icontroller++)
		{
		m_xboxControllers[icontroller]->update();
		}
	}

void InputSys::render()
	{
	//TODO: draw debug overlay of the keyboard & xbox controller states when F11 is pressed
	}

XboxController& InputSys::getXboxController(XboxControllerNumber icontroller)
	{
	return *m_xboxControllers[icontroller];
	}

void InputSys::initMousePosition(unsigned short mouseflags,const IntVector2& mousepos)
	{
	registerMouseMove(mouseflags,mousepos);
	m_mouseState.m_prevpos=m_mouseState.m_pos;
	}

void InputSys::stabilizeMousePos()
	{
	m_mouseState.stabilize();
	}

void InputSys::registerMouseMove(unsigned short mouseflags,const IntVector2& mousepos)
	{
	registerMouseButtonDown(mouseflags,mousepos);
	}

IntVector2 InputSys::calcMouseMoveDisplacement()
{
	return m_mouseState.m_pos-m_mouseState.m_prevpos;
	}

void InputSys::registerMouseButtonDown(unsigned short mouseflags,const IntVector2& mousepos)
	{
	unsigned short& mbstates = m_mouseState.m_buttonDownStates;
	mbstates=0;
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::LEFT ? MOUSE_BUTTON::LEFT : 0);
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::RIGHT ? MOUSE_BUTTON::RIGHT : 0);
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::MIDDLE ? MOUSE_BUTTON::MIDDLE : 0);

	registerMousePositionChange(mousepos);
	}

void InputSys::registerMouseButtonUp(unsigned short mouseflags,const IntVector2& mousepos)
	{
	unsigned short& mbstates = m_mouseState.m_buttonUpStates;
	mbstates=0;
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::LEFT ? MOUSE_BUTTON::LEFT : 0);
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::RIGHT ? MOUSE_BUTTON::RIGHT : 0);
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::MIDDLE ? MOUSE_BUTTON::MIDDLE : 0);

	registerMousePositionChange(mousepos);
	}

void InputSys::registerMouseDoubleClick(unsigned short mouseflags,const IntVector2& mousepos)
	{
	unsigned short& mbstates = m_mouseState.m_buttonDoubleClickStates;
	mbstates=0;
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::LEFT ? MOUSE_BUTTON::LEFT : 0);
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::RIGHT ? MOUSE_BUTTON::RIGHT : 0);
	mbstates = mbstates | (mouseflags && MOUSE_BUTTON::MIDDLE ? MOUSE_BUTTON::MIDDLE : 0);

	registerMousePositionChange(mousepos);
	}

void InputSys::registerMousePositionChange(const IntVector2& mousepos)
	{
	m_mouseState.m_prevpos=m_mouseState.m_pos;
	m_mouseState.m_pos=mousepos;
	}

bool InputSys::wasMouseJustMoved()
	{
	bool result=m_mouseState.m_pos!=m_mouseState.m_prevpos;
	return result;
	}

bool InputSys::isMouseButtonDown(unsigned short buttonflag)
	{
	bool result = m_mouseState.m_buttonDownStates&&buttonflag != 0;
	return result;
	}

bool InputSys::wasMouseButtonJustPressed(unsigned short buttonflag)
	{
	bool result = m_mouseState.m_buttonUpStates&&buttonflag != 0;
	return result;
	}

bool InputSys::wasMouseButtonDoubleClicked(unsigned short buttonflag)
	{
	bool result = m_mouseState.m_buttonDoubleClickStates&&buttonflag != 0;
	return result;
	}

void InputSys::showMouseCursor(bool isNowVisible)
	{
	if (isNowVisible)
		{
		int showcount;
		showcount=ShowCursor(TRUE);
		while (showcount>0)
			{
			showcount=ShowCursor(FALSE);
			} while (showcount>0);
		}
	else
		{
		int showcount;
		showcount=ShowCursor(TRUE);
		while(showcount>-1)
			{
			showcount=ShowCursor(FALSE);
			}
		}

	}

void MouseState::update(float deltaSeconds)
	{
	static float mouseTimerSeconds=0.f;
	mouseTimerSeconds+=deltaSeconds;
	const float MOUSE_RESET_INTERVAL_SECONDS = 0.25f;
	if (mouseTimerSeconds>MOUSE_RESET_INTERVAL_SECONDS)
		{
		stabilize();
		resetButtons();
		mouseTimerSeconds=0.f;
		}
	}

void MouseState::stabilize()
	{
	m_prevpos=m_pos;
	}

void MouseState::resetButtons()
	{
	m_buttonDoubleClickStates=m_buttonDownStates=m_buttonUpStates=0;
	}

MouseState::MouseState()

	: m_pos(IntVector2::ZERO)
	,m_prevpos(IntVector2::ZERO)
	,m_buttonDownStates(0)
	,m_buttonUpStates(0)
	,m_buttonDoubleClickStates(0)
	{

	}
void InputSys::resetMouseButtons()
	{
	m_mouseState.resetButtons();
	}

