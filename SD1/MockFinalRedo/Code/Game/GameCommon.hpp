#pragma once;

#include "Engine/Input/InputSys.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"

extern AudioSystem* g_theAudio;
extern InputSys* g_theInput;
extern Renderer* g_theRenderer;
extern AABB2* g_theWorldBounds;
extern App* g_theApp;
extern Game* g_theGame;
extern SpriteSheet* g_sprites;

extern const float g_worldWidth;
extern const float g_worldHeight;

extern bool g_doShowDebugGraphics;

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