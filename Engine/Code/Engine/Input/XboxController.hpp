#pragma once

#include <Windows.h>
#include <Xinput.h> // include the Xinput API
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; using 9_1_0 explicitly
#include "Engine/Input/KeyState.hpp"
#include "Engine/Math/Vector2.hpp"

enum XboxButtonBit
	{
	XBUTTON_UP = 0,
	XBUTTON_DOWN,
	XBUTTON_LEFT,
	XBUTTON_RIGHT,
	XBUTTON_START,
	XBUTTON_BACK,
	XBUTTON_LSTICK,
	XBUTTON_RSTICK,
	XBUTTON_LB,
	XBUTTON_RB,
	//XBUTTON_BLACK,
	//XBUTTON_WHITE,
	XBUTTON_A=12,
	XBUTTON_B,
	XBUTTON_X,
	XBUTTON_Y,
	AMT_XBOX_BUTTONS
	};

const XboxButtonBit XBOX_FIRST_BUTTON = XBUTTON_UP;

enum XboxControllerNumber
	{
	XCONTROLLER_0 = 0,
	XCONTROLLER_1,
	XCONTROLLER_2,
	XCONTROLLER_3,
	AMT_XBOX_CONTROLLERS
	};

enum XboxStick
	{
	XSTICK_LEFT,
	XSTICK_RIGHT,
	AMT_XBOX_STICKS
	};

const float STICK_DEAD_ZONE_INNER = 8000.f;
const float STICK_DEAD_ZONE_OUTER = 30000.f;

enum XboxTrigger
	{
	XBOX_TRIGGER_LEFT,
	XBOX_TRIGGER_RIGHT,
	AMT_XBOX_CONTROLLER_TRIGGERS = 2
	};

class XboxController
	{
	private:
		XboxControllerNumber m_number;
		Vector2 m_analogStickStates[AMT_XBOX_STICKS];
		XINPUT_STATE m_state;
		DWORD m_errorStatus;
		XINPUT_STATE m_xboxControllerState;
		KeyState m_buttonStates[AMT_XBOX_BUTTONS];
		float m_triggers[AMT_XBOX_CONTROLLER_TRIGGERS];
	public:
		XboxController(XboxControllerNumber number);
		bool isXBoxButtonPressed(XboxButtonBit button);
		void registerXboxButtonUp(XboxButtonBit button);
		void registerXboxButtonDown(XboxButtonBit button);
		bool wasXboxButtonJustPressed(XboxButtonBit button);
		void updateStates();
		void update();
		void updateButtonPresses();
		void updateTriggers();
		void updateSticks();
		void resetJustChangedStates();
		float getStickValue(XboxStick istick,Axis2 iaxis);
		float getTriggerValue(XboxTrigger whichTrigger);
		float stickRangeMap(float rawStickValue);
	};

