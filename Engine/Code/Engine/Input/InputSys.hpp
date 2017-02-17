#pragma once

#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/KeyState.hpp"
#include "Engine/Math/IntVector2.hpp"

enum KeyCode
	{
	KEYCODE_LMOUSEBUTTON = 0x0,
	KEYCODE_RMOUSEBUTTON = 0x1,
	KEYCODE_XBUTTON1 = 0x05,    /* NOT contiguous with L & RBUTTON */
	KEYCODE_XBUTTON2 = 0x06,    /* NOT contiguous with L & RBUTTON */
	KEYCODE_BACK = 0x08,
	KEYCODE_TAB = 0x09,
	KEYCODE_CLEAR = 0x0C,
	KEYCODE_RETURN = 0x0D,
	KEYCODE_SHIFT = 0x10,
	KEYCODE_CONTROL = 0x11,
	KEYCODE_MENU = 0x12,
	KEYCODE_PAUSE = 0x13,
	KEYCODE_CAPITAL = 0x14,
	KEYCODE_KANA = 0x15,
	KEYCODE_HANGEUL = 0x15,  /* old name - should be here for compatibility */
	KEYCODE_HANGUL = 0x15,
	KEYCODE_JUNJA = 0x17,
	KEYCODE_FINAL = 0x18,
	KEYCODE_HANJA = 0x19,
	KEYCODE_KANJI = 0x19,
	KEYCODE_ESCAPE = 0x1B,
	KEYCODE_CONVERT = 0x1C,
	KEYCODE_NONCONVERT = 0x1D,
	KEYCODE_ACCEPT = 0x1E,
	KEYCODE_MODECHANGE = 0x1F,
	KEYCODE_SPACE = 0x20,
	KEYCODE_PRIOR = 0x21,
	KEYCODE_NEXT = 0x22,
	KEYCODE_END = 0x23,
	KEYCODE_HOME = 0x24,
	KEYCODE_LEFT = 0x25,
	KEYCODE_UP = 0x26,
	KEYCODE_RIGHT = 0x27,
	KEYCODE_DOWN = 0x28,
	KEYCODE_SELECT = 0x29,
	KEYCODE_PRINT = 0x2A,
	KEYCODE_EXECUTE = 0x2B,
	KEYCODE_SNAPSHOT = 0x2C,
	KEYCODE_INSERT = 0x2D,
	KEYCODE_DELETE = 0x2E,
	KEYCODE_HELP = 0x2F,
	KEYCODE_LWIN = 0x5B,
	KEYCODE_RWIN = 0x5C,
	KEYCODE_APPS = 0x5D,
	KEYCODE_SLEEP = 0x5F,
	KEYCODE_NUMPAD0 = 0x60,
	KEYCODE_NUMPAD1 = 0x61,
	KEYCODE_NUMPAD2 = 0x62,
	KEYCODE_NUMPAD3 = 0x63,
	KEYCODE_NUMPAD4 = 0x64,
	KEYCODE_NUMPAD5 = 0x65,
	KEYCODE_NUMPAD6 = 0x66,
	KEYCODE_NUMPAD7 = 0x67,
	KEYCODE_NUMPAD8 = 0x68,
	KEYCODE_NUMPAD9 = 0x69,
	KEYCODE_MULTIPLY = 0x6A,
	KEYCODE_ADD = 0x6B,
	KEYCODE_SEPARATOR = 0x6C,
	KEYCODE_SUBTRACT = 0x6D,
	KEYCODE_DECIMAL = 0x6E,
	KEYCODE_DIVIDE = 0x6F,
	KEYCODE_F1 = 0x70,
	KEYCODE_F2 = 0x71,
	KEYCODE_F3 = 0x72,
	KEYCODE_F4 = 0x73,
	KEYCODE_F5 = 0x74,
	KEYCODE_F6 = 0x75,
	KEYCODE_F7 = 0x76,
	KEYCODE_F8 = 0x77,
	KEYCODE_F9 = 0x78,
	KEYCODE_F10 = 0x79,
	KEYCODE_F11 = 0x7A,
	KEYCODE_F12 = 0x7B,
	KEYCODE_F13 = 0x7C,
	KEYCODE_F14 = 0x7D,
	KEYCODE_F15 = 0x7E,
	KEYCODE_F16 = 0x7F,
	KEYCODE_F17 = 0x80,
	KEYCODE_F18 = 0x81,
	KEYCODE_F19 = 0x82,
	KEYCODE_F20 = 0x83,
	KEYCODE_F21 = 0x84,
	KEYCODE_F22 = 0x85,
	KEYCODE_F23 = 0x86,
	KEYCODE_F24 = 0x87,
	KEYCODE_NUMLOCK = 0x90,
	KEYCODE_SCROLL = 0x91,
	KEYCODE_OEM_NEC_EQUAL = 0x92,   // '=' key on numpad
	KEYCODE_OEM_FJ_JISHO = 0x92,   // 'Dictionary' key
	KEYCODE_OEM_FJ_MASSHOU = 0x93,   // 'Unregister word' key
	KEYCODE_OEM_FJ_TOUROKU = 0x94,   // 'Register word' key
	KEYCODE_OEM_FJ_LOYA = 0x95,   // 'Left OYAYUBI' key
	KEYCODE_OEM_FJ_ROYA = 0x96,   // 'Right OYAYUBI' key
	KEYCODE_LSHIFT = 0xA0,
	KEYCODE_RSHIFT = 0xA1,
	KEYCODE_LCONTROL = 0xA2,
	KEYCODE_RCONTROL = 0xA3,
	KEYCODE_LMENU = 0xA4,
	KEYCODE_RMENU = 0xA5,
	KEYCODE_BROWSER_BACK = 0xA6,
	KEYCODE_BROWSER_FORWARD = 0xA7,
	KEYCODE_BROWSER_REFRESH = 0xA8,
	KEYCODE_BROWSER_STOP = 0xA9,
	KEYCODE_BROWSER_SEARCH = 0xAA,
	KEYCODE_BROWSER_FAVORITES = 0xAB,
	KEYCODE_BROWSER_HOME = 0xAC,
	KEYCODE_VOLUME_MUTE = 0xAD,
	KEYCODE_VOLUME_DOWN = 0xAE,
	KEYCODE_VOLUME_UP = 0xAF,
	KEYCODE_MEDIA_NEXT_TRACK = 0xB0,
	KEYCODE_MEDIA_PREV_TRACK = 0xB1,
	KEYCODE_MEDIA_STOP = 0xB2,
	KEYCODE_MEDIA_PLAY_PAUSE = 0xB3,
	KEYCODE_LAUNCH_MAIL = 0xB4,
	KEYCODE_LAUNCH_MEDIA_SELECT = 0xB5,
	KEYCODE_LAUNCH_APP1 = 0xB6,
	KEYCODE_LAUNCH_APP2 = 0xB7,
	KEYCODE_OEM_1 = 0xBA,   // ';:' for US
	KEYCODE_OEM_PLUS = 0xBB,   // '+' any country
	KEYCODE_OEM_COMMA = 0xBC,   // ',' any country
	KEYCODE_OEM_MINUS = 0xBD,   // '-' any country
	KEYCODE_OEM_PERIOD = 0xBE,   // '.' any country
	KEYCODE_OEM_2 = 0xBF,   // '/?' for US
	KEYCODE_OEM_3 = 0xC0,   // '`~' for US
	KEYCODE_OEM_4 = 0xDB,  //  '[{' for US
	KEYCODE_OEM_5 = 0xDC,  //  '\|' for US
	KEYCODE_OEM_6 = 0xDD,  //  ']}' for US
	KEYCODE_OEM_7 = 0xDE,  //  ''"' for US
	KEYCODE_OEM_8 = 0xDF,
	KEYCODE_OEM_AX = 0xE1,  //  'AX' key on Japanese AX kbd
	KEYCODE_OEM_102 = 0xE2,  //  "<>" or "\|" on RT 102-key kbd.
	KEYCODE_ICO_HELP = 0xE3,  //  Help key on ICO
	KEYCODE_ICO_00 = 0xE4,  //  00 key on ICO
	KEYCODE_ICO_CLEAR = 0xE6,
	KEYCODE_OEM_RESET = 0xE9,
	KEYCODE_OEM_JUMP = 0xEA,
	KEYCODE_OEM_PA1 = 0xEB,
	KEYCODE_OEM_PA2 = 0xEC,
	KEYCODE_OEM_PA3 = 0xED,
	KEYCODE_OEM_WSCTRL = 0xEE,
	KEYCODE_OEM_CUSEL = 0xEF,
	KEYCODE_OEM_ATTN = 0xF0,
	KEYCODE_OEM_FINISH = 0xF1,
	KEYCODE_OEM_COPY = 0xF2,
	KEYCODE_OEM_AUTO = 0xF3,
	KEYCODE_OEM_ENLW = 0xF4,
	KEYCODE_OEM_BACKTAB = 0xF5,
	KEYCODE_ATTN = 0xF6,
	KEYCODE_CRSEL = 0xF7,
	KEYCODE_EXSEL = 0xF8,
	KEYCODE_EREOF = 0xF9,
	KEYCODE_PLAY = 0xFA,
	KEYCODE_ZOOM = 0xFB,
	KEYCODE_NONAME = 0xFC,
	KEYCODE_PA1 = 0xFD,
	KEYCODE_OEM_CLEAR = 0xFE, 
	AMT_KEYBOARD_KEYS = 256
	};

namespace MOUSE_BUTTON
	{
	enum BUTTON_FLAGS
		{
		LEFT=0x1,
		RIGHT=0x2,
		MIDDLE=0x10,
		AMT=3
		};
	};

struct MouseState
	{
	IntVector2 m_pos;
	IntVector2 m_prevpos;
	unsigned short m_buttonUpStates;
	unsigned short m_buttonDownStates;
	unsigned short m_buttonDoubleClickStates;

	MouseState();
	void update(float deltaS);
	void stabilize();
	void resetButtons();
	};

class InputSys
	{
	public: //constructor and destructor
		InputSys();
		~InputSys();
	public: // methods
		void beginFrame();
		void endFrame();

		void onGainedFocus();
		void onLostFocus();
		bool doesHaveFocus() { return m_doesHaveFocus; }

		IntVector2 getWindowCenter();

		IntVector2 getCursorPos() const;
		void centerCursorPos();
		void setCursorPos(const IntVector2& newpos);
		void stabilizeMousePos();
		void resetMouseButtons();
		void initMousePosition(unsigned short mouseflags,const IntVector2& mousepos);
		void registerMouseMove(unsigned short mouseflags,const IntVector2& mousepos);
		IntVector2 calcMouseMoveDisplacement();
		void registerMouseButtonDown(unsigned short mouseflags,const IntVector2& mousepos);
		void registerMouseButtonUp(unsigned short mouseflags,const IntVector2& mousepos);
		void registerMouseDoubleClick(unsigned short mouseflags,const IntVector2& mousepos);
		void registerMousePositionChange(const IntVector2& mousepos);
		bool wasMouseJustMoved();
		bool isMouseButtonDown(unsigned short buttonflag);
		bool wasMouseButtonJustPressed(unsigned short buttonflag);
		bool wasMouseButtonDoubleClicked(unsigned short buttonflag);
		void showMouseCursor(bool isNowVisible);

		void registerKeyDown(unsigned char ikey);
		void registerKeyUp(unsigned char ikey);
		bool isKeyDown(unsigned char ikey);
		bool wasKeyJustPressed(unsigned char key);

		void update(float deltaSeconds);
		void render();
	public: //accessors
		XboxController& getXboxController(XboxControllerNumber icontroller);
	private:
		KeyState m_keystates[AMT_KEYBOARD_KEYS];
		XboxController* m_xboxControllers[AMT_XBOX_CONTROLLERS];
		MouseState m_mouseState;
		bool m_doesHaveFocus;
	public:
	};

