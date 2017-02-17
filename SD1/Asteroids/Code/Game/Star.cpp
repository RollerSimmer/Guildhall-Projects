#include "Game/Star.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Game/GameCommon.hpp"

const int AMT_STAR_COLORS=10;



const int AMT_STAR_RENDER_SIDES = 8;

Star::Star()
	: m_geometry()
	, m_color(0)
	{
	m_geometry.m_center.x = GetRandomFloatInRange(g_theWorldBounds->m_mins.x, g_theWorldBounds->m_maxs.x);
	m_geometry.m_center.y = GetRandomFloatInRange(g_theWorldBounds->m_mins.y, g_theWorldBounds->m_maxs.y);
	m_geometry.m_radius = GetRandomFloatInRange(0.f,MAX_STAR_RADIUS);
	float whiteScale= GetRandomFloatInRange(0.f,1.f);
	m_color=whiteScale*Rgba::WHITE + (1.f-whiteScale)*Rgba::YELLOW;
	m_color.scaleAlpha(GetRandomFloatInRange(0.5f,1.f));
	}

void Star::render()
	{
	g_theRenderer->drawDisc2(m_geometry.m_center.x,m_geometry.m_center.y,m_geometry.m_radius, AMT_STAR_RENDER_SIDES,m_color);
	}

void Star::renderStarLine(int frame)
	{
	float maxDist = g_theWorldBounds->calcWidth();
	float scale = (float)(frame + 1) / (float)(GAME_STATE_LAST_STAR_LINE_FRAME-GAME_STATE_FIRST_STAR_LINE_FRAME);
	Vector2 worldCenter=(g_theWorldBounds->CalcCenter());
	Vector2 lineDirection=m_geometry.m_center-worldCenter;
	lineDirection.SetLength(maxDist*scale);
	Vector2 startpoint=m_geometry.m_center;
	Vector2 endpoint=startpoint+lineDirection;
	g_theRenderer->drawLine(startpoint.x,startpoint.y,endpoint.x,endpoint.y,m_color,m_geometry.m_radius*2.f);
	}
