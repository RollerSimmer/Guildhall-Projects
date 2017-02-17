#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cstdlib>

void XboxController::updateButtonPresses()
	{
	for (int ibutton = 0; ibutton < AMT_XBOX_BUTTONS; ibutton++)
		{
		XboxButtonBit buttonbit=(XboxButtonBit)ibutton;
		if(isXBoxButtonPressed(buttonbit))
			registerXboxButtonDown(buttonbit);
		else
			registerXboxButtonUp(buttonbit);
		}
	}

void XboxController::updateTriggers()
	{
	m_triggers[XBOX_TRIGGER_LEFT] = (float)m_xboxControllerState.Gamepad.bLeftTrigger/255.f;
	m_triggers[XBOX_TRIGGER_RIGHT] = (float)m_xboxControllerState.Gamepad.bRightTrigger/255.f;
	}

void XboxController::updateSticks()
	{	
	Vector2 lstick(m_xboxControllerState.Gamepad.sThumbLX, m_xboxControllerState.Gamepad.sThumbLY);
	Vector2 rstick(m_xboxControllerState.Gamepad.sThumbRX, m_xboxControllerState.Gamepad.sThumbRY);
	float lstickLength=stickRangeMap(lstick.calcLength());
	float rstickLength=stickRangeMap(rstick.calcLength());
	lstick.setLength(lstickLength); 
	rstick.setLength(rstickLength);
	m_analogStickStates[XSTICK_LEFT]=lstick;
	m_analogStickStates[XSTICK_RIGHT]=rstick;
	}

void XboxController::resetJustChangedStates()
	{
	for (int ibutton = XBUTTON_UP; ibutton < AMT_XBOX_BUTTONS; ibutton++)
		{
		m_buttonStates[ibutton].m_justChanged=false;
		}
	}

float XboxController::getStickValue(XboxStick istick, Axis2 iaxis)
	{
	if (iaxis == X_AXIS_2D)
		{
		return m_analogStickStates[istick].x;
		}
	if (iaxis == Y_AXIS_2D)
		{
		return m_analogStickStates[istick].y;
		}
	return 0.f;
	}

float XboxController::getTriggerValue(XboxTrigger whichTrigger)
	{
	if(whichTrigger<AMT_XBOX_CONTROLLER_TRIGGERS)
		return m_triggers[whichTrigger];
	return 0.f;
	}

float XboxController::stickRangeMap(float rawStickValue)
	{
	float result=0.f;
	result=rangeMapFloat(rawStickValue, STICK_DEAD_ZONE_INNER, STICK_DEAD_ZONE_OUTER, 0.f, 1.f);
	result=clampFloat(result,0.f,1.f);
	return result;
	}

XboxController::XboxController(XboxControllerNumber number)
	: m_number(number)
	, m_errorStatus(0)
	{
	updateStates();
	}

bool XboxController::isXBoxButtonPressed(XboxButtonBit ibutton)
	{
	return (m_xboxControllerState.Gamepad.wButtons & ((unsigned short)1<<ibutton)) != 0;
	}

void XboxController::registerXboxButtonUp(XboxButtonBit ibutton)
	{
	if(m_buttonStates[ibutton].m_isDown)
		m_buttonStates[ibutton].m_justChanged=true;
	else
		m_buttonStates[ibutton].m_justChanged=false;
	m_buttonStates[ibutton].m_isDown=false;
	}

void XboxController::registerXboxButtonDown(XboxButtonBit ibutton)
	{
	if (!m_buttonStates[ibutton].m_isDown)
		m_buttonStates[ibutton].m_justChanged = true;
	else
		m_buttonStates[ibutton].m_justChanged = false;
	m_buttonStates[ibutton].m_isDown = true;
	}

bool XboxController::wasXboxButtonJustPressed(XboxButtonBit button)
	{
	return m_buttonStates[button].m_isDown && m_buttonStates[button].m_justChanged;
	}

void XboxController::updateStates()
	{
	memset(&m_xboxControllerState, 0, sizeof(m_xboxControllerState));
	m_errorStatus = XInputGetState(m_number, &m_xboxControllerState);
	}

void XboxController::update()
	{
	resetJustChangedStates();
	updateStates();
	updateButtonPresses();
 	updateTriggers();
	updateSticks();
	}
