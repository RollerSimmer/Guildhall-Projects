#include <cmath>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cassert>

const Vector2 Vector2::ZERO(0.f,0.f);
const Vector2 Vector2::ONE(1.f,1.f);

Vector2::Vector2()
	: x(0)
	, y(0)
	{

	}
			
Vector2::Vector2(const Vector2& copy)
	: x(copy.x)
	, y(copy.y) 
	{
	}

Vector2::Vector2(float initialX, float initialY)
	:x(initialX)
	,y(initialY)
	{
	}

Vector2::Vector2(const Vector3& truncVector)
	: x(truncVector.x)
	, y(truncVector.y)
	{

	}

Vector2::Vector2(const Vector4& truncVector)
	: x(truncVector.x)
	, y(truncVector.y)
	{

	}

void Vector2::GetXY( float& out_x, float& out_y ) const
	{
	out_x=x;
	out_y=y;
	}

void Vector2::SetXY(float newX, float newY)
{
	x = newX;
	y = newY;
}

const float* Vector2::GetAsFloatArray() const
	{
	return &x;
	}

float* Vector2::GetAsFloatArray()
	{
	return &x;
	}

float Vector2::calcLength() const
{
	return sqrt( calcLengthSquared() );
	}

float Vector2::octagonEstimateLength() const
	{
	const float optimalEdgeSlope = 0.262f;
	float xabs=absval(x);
	float yabs=absval(y);
	float m=xabs<yabs?xabs:yabs;
	float M=xabs>yabs?xabs:yabs;
	return M+optimalEdgeSlope*m;
	}

float Vector2::calcLengthSquared() const
	{
	return (x*x) + (y*y);
	}

float Vector2::calcHeadingDegrees() const
	{
	return ConvertRadiansToDegrees(CalcHeadingRadians());
	}

float Vector2::CalcHeadingRadians() const
	{
	return atan2(y,x);
	}

bool Vector2::operator == ( const Vector2& vectorToEqual ) const
	{
	return x==vectorToEqual.x && y==vectorToEqual.y;	
	}

bool Vector2::operator != ( const Vector2& vectorToNotEqual ) const
	{
	return x!=vectorToNotEqual.x || y!=vectorToNotEqual.y;	
	}

const Vector2 Vector2::operator + ( const Vector2& vectorToAdd ) const
	{
	Vector2 sum;
	sum.x=x+vectorToAdd.x;
	sum.y=y+vectorToAdd.y;
	return sum;
	}

const Vector2 Vector2::operator - ( const Vector2& vectorToSubtract ) const
	{
	Vector2 subres;
	subres.x=x-vectorToSubtract.x;
	subres.y=y-vectorToSubtract.y;
	return subres;
	}

Vector2 Vector2::operator -() const
	{
	return ZERO-(*this);
	}

const Vector2 Vector2::operator * ( float scale ) const
	{
	Vector2 prod;
	prod.x=x*scale;
	prod.y=y*scale;
	return prod;	
	}

const Vector2 Vector2::operator * ( const Vector2& perAxisScaleFactors ) const
	{
	Vector2 prod;
	prod.x=x*perAxisScaleFactors.x;
	prod.y=y*perAxisScaleFactors.y;
	return prod;		
	}

const Vector2 Vector2::operator / ( float inverseScale ) const
	{
	float scale=1/inverseScale;
	return (*this) * scale;		
	}

const Vector2 Vector2::operator / (const Vector2& vectorToDivide) const
{
	Vector2 result = *this;
	assert(x!=0.f && y!=0.f);
	result.y /= vectorToDivide.y;
	result.x /= vectorToDivide.x;
	return result;
}

const Vector2& Vector2::operator*=(const Vector2& perAxisScaleFactors)
	{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	return *this;
	}

const Vector2& Vector2::operator*=(float scale) 
	{
	x *= scale;
	y *= scale;
	return *this;
	}

void Vector2::operator += ( const Vector2& vectorToAdd )
	{
	if (isNan(x) || isNan(y))
		return;

	x+=vectorToAdd.x;
	y+=vectorToAdd.y;	

	if(isNan(x) || isNan(y))
		return;
	}

void Vector2::operator -= ( const Vector2& vectorToSubtract )
	{
	x-=vectorToSubtract.x;
	y-=vectorToSubtract.y;		
	}
	
float CalcDistance( const Vector2& positionA, const Vector2& positionB )
	{
	return sqrt(CalcDistanceSquared(positionA, positionB));
	}

float CalcDistanceSquared( const Vector2& posA, const Vector2& posB )
	{
	Vector2 d=posB-posA;
	return d.calcLengthSquared();
	}

const Vector2 operator * ( float scale, const Vector2& vectorToScale )
	{
	Vector2 result=vectorToScale*scale;
	return result;
	}

bool IsMostlyEqual(const Vector2& a, const Vector2& b)
{
	bool resultX = IsMostlyEqual(a.x, b.x);
	bool resultY = IsMostlyEqual(a.y, b.y);
	return resultX && resultY;
}


void Vector2::Rotate90Degrees()
{
	float newx = -y;
	float newy = x;
	x = newx;
	y = newy;
}

void Vector2::RotateNegative90Degrees()
{
	float newx = y;
	float newy = -x;
	x = newx;
	y = newy;
}

void Vector2::RotateDegrees(float degrees)
{
	RotateRadians(ConvertDegreesToRadians(degrees));
}

void Vector2::RotateRadians(float radians)
{
	float curHeadingRadians = atan2(y, x);
	float newHeadingRadians = curHeadingRadians + radians;
	float R = this->calcLength();
	x = R*cos(newHeadingRadians);
	y = R*sin(newHeadingRadians);
}

float Vector2::Normalize()
	{
	float len = this->calcLength();
	if (len==0.f)
		return 0;
	float invLength = 1.f/len;
	x *= invLength;
	y *= invLength;
	return invLength;
	}

float Vector2::setLength(float newLength)
	{
	float oldLength = this->calcLength();
	if(oldLength==0.f)
		return 0;
	float scale = newLength / oldLength;
	this->ScaleUniform(scale);
	return oldLength;
	}

void Vector2::ScaleUniform(float scale)
	{
	x *= scale;
	y *= scale;
	}

void Vector2::ScaleNonUniform(const Vector2& perAxisScaleFactors)
	{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	}

void Vector2::InverseScaleNonUniform(const Vector2& perAxisDivisors)
	{
	if (perAxisDivisors.x == 0.f || perAxisDivisors.y == 0.f)
		return;
	x /= perAxisDivisors.x;
	y /= perAxisDivisors.y;
	}

void Vector2::SetHeadingDegrees(float headingDegrees)
{
	SetHeadingRadians(ConvertDegreesToRadians(headingDegrees));
}

void Vector2::SetHeadingRadians(float headingRadians)
{
	float R = this->calcLength();
	x = R*cos(headingRadians);
	y = R*sin(headingRadians);
}

Vector2 Vector2::getNormalized()
	{
	Vector2 result=*this;
	result.Normalize();
	return result;
	}

Vector2 Vector2::calcPortionOfLength(float length)
	{
	Vector2 result = *this;
	result.setLength(length);
	return result;
	}

void Vector2::SetUnitLengthAndHeadingDegrees(float headingDegrees)
{
	this->Normalize();
	this->SetHeadingDegrees(headingDegrees);
}

void Vector2::SetUnitLengthAndHeadingRadians(float headingRadians)
{
	this->Normalize();
	this->SetHeadingRadians(headingRadians);
}

void Vector2::SetLengthAndHeadingDegrees(float newLength, float	headingDegrees)
{
	this->setLength(newLength);
	this->SetHeadingDegrees(headingDegrees);
}

void Vector2::SetLengthAndHeadingRadians(float newLength, float	headingRadians)
{
	this->setLength(newLength);
	this->SetHeadingRadians(headingRadians);
}

void Vector2::setLengthX(float length)
	{
	if (x == 0.f)
		{
		x = length;
		}
	else
		{
		float scale = length / abs(x);
		x *= scale;
		}
	}

void Vector2::setLengthY(float length)
	{
	if (y == 0.f)
		{
		y = length;
		}
	else
		{
		float scale = length / abs(y);
		y *= scale;
		}
	}

float Vector2::dotProduct(const Vector2& other) const
	{
	return x*other.x + y*other.y;
	}

Vector2 Vector2::getRandomNormalizedVector()
	{
	float length=1.f;
	Vector2 result=getRandomVector(length);
	return result;
	}

Vector2 Vector2::getRandomVector(float length/*=1.f*/)
	{
	Vector2 result=Vector2(getRandomSine(), getRandomSine());
	result.setLength(length);
	return result;
	}

Vector2 makeVector2(float vx, float vy)
	{
	Vector2 result;
	result.x=vx;
	result.y=vy;
	return result;
	}

float dotProduct(const Vector2& a, const Vector2& b)
	{
	return a.dotProduct(b);
	}

Vector2 interpolate(float scaleA, const Vector2& a, float scaleB, const Vector2& b)
	{
	Vector2 acopy(a),bcopy(b);
	Vector2 result = acopy*scaleA+bcopy*scaleB;
	return result;
	}