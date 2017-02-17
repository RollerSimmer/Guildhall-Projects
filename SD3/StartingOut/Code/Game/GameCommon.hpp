#pragma once;
#include "Game/ConfigSettings.hpp"

class InputSys;
class Renderer;
class AABB2;
class Window;
class App;

extern App* g_theApp;
extern InputSys* g_theInput;
extern Renderer* g_theRenderer;
extern AABB2* g_theWorldBounds;
extern Window* g_theWindow;

extern const float g_worldWidth;
extern const float g_worldHeight;

extern bool g_doShowDebugGraphics;

const int DEFAULT_WINDOW_WIDTH = 1024;
const int DEFAULT_WINDOW_HEIGHT = 768;

//-----------------------------------------------------------------------------------------------
const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const double VIEW_LEFT = 0.0;
const double VIEW_RIGHT = g_worldWidth;
const double VIEW_BOTTOM = 0.0;
const double VIEW_TOP = g_worldHeight;
//const double VIEW_TOP = 0.0;


extern ConfigSettings g_configSettings;

enum GameStates
	{
	GAME_STATE_PLAYING = -2,
	GAME_STATE_MOVING_TO_CENTER_FOR_END = -1,
	GAME_STATE_FIRST_STAR_LINE_FRAME = 0,
	GAME_STATE_LAST_STAR_LINE_FRAME = 100,
	GAME_STATE_FIRST_PLANET_FRAME = 101,
	GAME_STATE_LAST_PLANET_FRAME = 200,
	GAME_STATE_PLANET_ARRIVAL_STAGE = 201,
	GAME_STATE_GAME_OVER_STAGE = 300
	};

extern const float END_MOVIE_FRAME_SECONDS;