#pragma once

#include "Engine/Math/Vector2.hpp"

const float INFINITE_MASS = 0.f;

void transferMomentumGeneric(float massA,Vector2& velA,const Vector2& posA, float elasticityA,
					  float massB, Vector2& velB, const Vector2& posB, float elasticityB);
