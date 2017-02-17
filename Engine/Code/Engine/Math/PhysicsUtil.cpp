#include "Engine/Math/PhysicsUtil.hpp"
#include "Engine/Math/MathUtil.hpp"

// code written by John Anderson

void transferMomentumGeneric(float massA, Vector2& velA, const Vector2& posA, float elasticityA, float massB, Vector2& velB, const Vector2& posB, float elasticityB)
	{
	//TODO: fix "sticking" issues when balls cling instead of rolling off smoothly.

	Vector2 oldAmo = massA*velA;
	Vector2 oldBmo = massB*velB;
	Vector2 totalMo=oldAmo+oldBmo;
	Vector2 L=posB-posA;
	L.Normalize();
	Vector2 oldAmoAlongL = L*dotProduct(oldAmo, L);
	Vector2 oldBmoAlongL = L*dotProduct(oldBmo, L);
	Vector2 oldTotalMoAlongL=oldAmoAlongL+oldBmoAlongL;
	//float oldTotalMagAlongL=oldTotalMoAlongL.CalcLength();

	// filter out very slow speeds
		{
		static float velsum=0;
		static int counter=0;
		float totalVel = velA.calcLength() + velB.calcLength();
		velsum+=totalVel;
		counter++;
		if (counter >= 10000)
			{
			//float avgvel=velsum/counter;
			counter=0;
			}
		const float VELOCITY_THRESHOLD = 1000.f;
		if (totalVel < VELOCITY_THRESHOLD)
			return;
		}

	Vector2 AmoAlongN = oldAmo-oldAmoAlongL;
	Vector2 BmoAlongN = oldBmo-oldBmoAlongL;
	Vector2 newAmoAlongL = oldBmoAlongL;
	Vector2 newBmoAlongL = oldAmoAlongL;
	Vector2 newAmo;
	Vector2 newBmo;
	float newAvelScale;
	float newBvelScale;
	if (massA == INFINITE_MASS && massB == INFINITE_MASS)
		{
		// set both momentums to zero
		newAmo = newBmo = Vector2::ZERO;
		newAvelScale = newBvelScale = 0.f;
		}
	else if (massA == INFINITE_MASS)
		{
		newAmo = Vector2::ZERO;
		newBmo = -oldBmoAlongL+BmoAlongN;
		newAvelScale = 0.f;
		newBvelScale = 1.f / massB;
		}
	else if (massB == INFINITE_MASS)
		{
		newBmo = Vector2::ZERO;		
		newAmo = -oldAmoAlongL+AmoAlongN;
		newBvelScale = 0.f;
		newAvelScale = 1.f / massA;
		}
	else
		{
		newAmo = AmoAlongN + newAmoAlongL;
		newBmo = BmoAlongN + newBmoAlongL;
		newAvelScale = 1.f / massA;
		newBvelScale = 1.f / massB;
		}
	velA = newAmo*newAvelScale*elasticityA;
	velB = newBmo*newBvelScale*elasticityB;
	}