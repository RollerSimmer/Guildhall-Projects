#pragma once

#include "Engine/Input/InputSys.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Randomizer.hpp"

extern InputSys* g_theInput;
extern Renderer* g_theRenderer;
extern AABB2* g_theWorldBounds;

const float WORLD_SIZE = 800.f;
const float WORLD_WIDTH = WORLD_SIZE;
const float WORLD_HEIGHT = WORLD_SIZE;
const float PACHINKO_MACHINE_SIZE_IN_METERS = 2.f;
extern float g_worldGravitationalAcceleration;
extern float g_projectileTerminalSpeed;
extern float g_projectileElasticity;
extern float g_obstacleElasticity;

extern bool g_doShowDebugGraphics;

enum ProjectileRenderType
	{
	PROJ_RENDER_TYPE_SOLID,
	PROJ_RENDER_TYPE_LIQUID
	};

extern ProjectileRenderType g_projectileRenderType;

