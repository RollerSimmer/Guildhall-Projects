#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/RgbaFloat.hpp"

struct Vertex3
	{
	Vector3 m_pos;
	Rgba m_color;
	Vector2 m_texcoords;
	Vertex3(): m_pos(Vector3::ZERO), m_color(Rgba::WHITE) {}
	Vertex3(const Vector3& pos, const Rgba& color, const Vector2& texcoords): m_pos(pos), m_color(color), m_texcoords(texcoords) {}
	};

struct Vertex3F
	{
	Vector3 m_pos;
	RgbaFloat m_color;
	Vector2 m_texcoords;
	Vertex3F(): m_pos(Vector3::ZERO), m_color(1.f,1.f,1.f,1.f) {}
	Vertex3F(const Vector3& pos, const RgbaFloat& color, const Vector2& texcoords): m_pos(pos), m_color(color), m_texcoords(texcoords) {}
	};