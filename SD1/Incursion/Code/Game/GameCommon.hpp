#pragma once;

#include "Engine/Input/InputSys.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/Map.hpp"
#include "Game/App.hpp"
#include "Engine/Audio/Audio.hpp"

extern InputSys* g_theInput;
extern Renderer* g_theRenderer;
extern AABB2* g_theWorldBounds;
extern Map* g_theMap;
extern App* g_theApp;
extern AudioSystem* g_theAudio;

extern const float g_screenWidth;
extern const float g_screenHeight;

extern bool g_doShowDebugGraphics;

extern float g_totalElapsedSecondsSinceStart;