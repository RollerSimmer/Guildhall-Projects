#include <iostream>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Matrix4.hpp"
using namespace std;


void validate(char* message, bool isSuccessful)
	{
	cout << "Validating " << message << "." << endl;
	if (isSuccessful)
		printf("Success.\n");
	else
		printf("FAILED!\n");
	}


int main()
	{
	Vector2 s_p,s_n;
	s_p=Vector2(4.f,0.f);
	cout<<"s.x\ts.y"<<endl;
	cout<<s_p.x<<"\t"<<s_p.y<<endl;
	float m=1.f;
	float m_inv=1.f/m;
	float k=1.f;
	float c=0.1f;

	float dt=0.1f;
	for (float t=0.f;t<10.f;t+=dt)
		{

		float det=m/(m+c*dt+k*dt*dt);
		s_n.x=det*m_inv*((m+c*dt)*s_p.x-k*dt*s_p.y);
		s_n.y=det*(dt*s_p.x+1*s_p.y);
		cout<<s_n.x<<"\t"<<s_n.y<<endl;
		s_p=s_n;
		}
	}