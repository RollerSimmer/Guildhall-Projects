#pragma once;

#include "Engine/Input/InputSys.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/DebugMessagePrinter.hpp"
#include "Engine/Audio/Audio.hpp"
class Game;
class World;

extern AudioSystem* g_theAudioSys;
extern InputSys* g_theInput;
extern Renderer* g_theRenderer;
extern AABB2* g_theWorldBounds;
extern SpriteSheet* g_tileAtlas;
extern Game* g_theGame;
extern World* g_theWorld;
extern BitmapFont* g_theFont;
extern DebugMessagePrinter* g_theDebugMessagePrinter;

extern int g_waterLevel;

extern const float g_worldWidth;
extern const float g_worldHeight;

extern bool g_doShowDebugGraphics;

constexpr int CHUNK_X_BITS = 3;
constexpr int CHUNK_Y_BITS = 3;
constexpr int CHUNK_Z_BITS = 6;
constexpr int CHUNK_X_MASK_OFFSET = 0;
constexpr int CHUNK_Y_MASK_OFFSET = CHUNK_X_BITS;
constexpr int CHUNK_Z_MASK_OFFSET = CHUNK_Y_MASK_OFFSET+CHUNK_Y_BITS;
constexpr int CHUNK_BLOCKS_X = BIT(CHUNK_X_BITS);
constexpr int CHUNK_BLOCKS_Y = BIT(CHUNK_Y_BITS);
constexpr int CHUNK_BLOCKS_Z = BIT(CHUNK_Z_BITS);
constexpr float MAX_GENERATED_HEIGHT_PCT = 0.55f;
constexpr int MAX_GENERATED_HEIGHT = (int)((float)CHUNK_BLOCKS_Z*MAX_GENERATED_HEIGHT_PCT);
constexpr int X_MASK_BITS_UNSHIFTED = MASK(CHUNK_X_BITS);
constexpr int Y_MASK_BITS_UNSHIFTED = MASK(CHUNK_Y_BITS);
constexpr int Z_MASK_BITS_UNSHIFTED = MASK(CHUNK_Z_BITS);
constexpr int X_MASK_BITS = X_MASK_BITS_UNSHIFTED << CHUNK_X_MASK_OFFSET;
constexpr int Y_MASK_BITS = Y_MASK_BITS_UNSHIFTED << CHUNK_Y_MASK_OFFSET;
constexpr int Z_MASK_BITS = Z_MASK_BITS_UNSHIFTED << CHUNK_Z_MASK_OFFSET;
const int BLOCKS_PER_LAYER = CHUNK_BLOCKS_Y*CHUNK_BLOCKS_X;
const int BLOCKS_PER_CHUNK=CHUNK_BLOCKS_X*CHUNK_BLOCKS_Y*CHUNK_BLOCKS_Z;

const int LIGHT_LEVEL_MOONLIGHT = 6;
const int LIGHT_LEVEL_SUNLIGHT = 15;

const float WORLD_GRAVITATIONAL_ACCELERATION_METERS_PER_SECOND=9.8f;
const float WORLD_TERMINAL_SPEED_METERS_PER_SECOND=10.f;
const float JUMP_SPEED = 8.f;
const float AUTO_JUMP_SPEED = 5.2f;
const float MOVEMENT_FRICTION_DECELERATION_COEFFICIENT=0.9f;
const float WORLD_TERMINAL_SPEED_METERS_PER_SECOND_SQUARED=WORLD_TERMINAL_SPEED_METERS_PER_SECOND
                                                          *WORLD_TERMINAL_SPEED_METERS_PER_SECOND;
const float MOVE_SPEED = 15.f;
const float MOVE_ACCELERATION_RATE = 7.f;
const float FLY_ASCENSION_ACCELERATION_RATE=5.5f;
const float AUTO_JUMP_COOLDOWN_DURATION_SECONDS=0.25f;

extern bool operator < (const IntVector2& a,const IntVector2& b);

struct Rect3
	{
	Vector3 points[4];
	void operator +=(const Vector3& translation)
		{
		for (int i=0;i<4;i++)
			points[i]+=translation;
		}
	Vector3 calcCenter()
		{
		Vector3 result=Vector3::ZERO;
		for (int i=0;i<4;i++)
			result+=points[i];
		result*=0.25f;
		return result;
		}
	void operator *=(float scale)
		{
		Vector3 center=calcCenter();
		for (int i=0;i<4;i++)
			{
			points[i]-=center;
			points[i]*=scale;
			points[i]+=center;
			}
		}
	};

