#include <iostream>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Matrix4.hpp"
using namespace std;

class MySystem
	{
	public:

		float m1,k1,c1,l1,
			  m2,k2,c2,l2;
		double g;
		double c_env;

		MySystem(){}
		float calcAccelForFirst(float t,const Vector4& state);
		float calcAccelForSecond(float t,const Vector4& state);
		Vector4 operator() (float t,const Vector4& state);
	};

float MySystem::calcAccelForFirst(float t,const Vector4& state)
	{
	float result;
	float F_k1=-k1*(state[0]-l1);
	float F_c1=-c1*state[1];
	float F_k2=k2*((state[2]-state[0])-l2);
	float F_c2=c2*(state[3]-state[1]);
	float F_g=m1*g;
	return result;
	}

float MySystem::calcAccelForSecond(float t,const Vector4& state)
	{
	float result;
	float F_k1=-k1*(state[0]-l1);
	float F_c1=-c1*state[1];
	float F_k2=k2*((state[2]-state[0])-l2);
	float F_c2=c2*(state[3]-state[1]);
	float F_g=m1*g;
	return result;
	}

Vector4 MySystem::operator() (float t,const Vector4& state)
	{
	Vector4 deriv;
	deriv[0]=state[1];
	deriv[2]=state[3];
	deriv[1]=calcAccelForFirst(t,state);
	deriv[3]=calcAccelForSecond(t,state);
	}



int main()
	{

	system("pause");
	return 0;
	}