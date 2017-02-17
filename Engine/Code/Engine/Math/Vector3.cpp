#include "Engine/Math/Vector3.hpp"
#include <cmath>
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/IntVector3.hpp"
#include <cassert>
#include <sstream>

const Vector3 Vector3::X_AXIS_VECTOR(1.f, 0.f, 0.f);
const Vector3 Vector3::Y_AXIS_VECTOR(0.f, 1.f, 0.f);
const Vector3 Vector3::Z_AXIS_VECTOR(0.f, 0.f, 1.f);
const Vector3 Vector3::ZERO(0.f, 0.f, 0.f);

Vector3::Vector3(float setx, float sety, float setz)
	: x(setx)
	, y(sety)
	, z(setz)
	{

	}

Vector3::Vector3(const Vector3& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	{

	}

Vector3::Vector3()
	: x(0.f)
	, y(0.f)
	, z(0.f)
	{

	}


Vector3::Vector3(const Vector2& xy,float setz)
	: x(xy.x)
	, y(xy.y)
	, z(setz)
	{

	}

Vector3::Vector3(const Vector4& truncVector)
	: x(truncVector.x)
	, y(truncVector.y)
	, z(truncVector.z)
	{

	}

Vector3::Vector3(const IntVector3& intVector)
	: x((float)intVector.x)
	, y((float)intVector.y)
	, z((float)intVector.z)
	{

	}

void Vector3::getXYZ(float& xout, float& yout, float& zout) const
	{
	xout = x;
	yout = y;
	zout = z;
	}

const float* Vector3::getAsFloatArray() const
	{
	return (float*)&x;
	}

float* Vector3::getAsFloatArray()
	{
	return (float*)&x;
	}

float Vector3::calcLength() const
	{
	return sqrt(calcLengthSquared());
	}

float Vector3::octagonEstimateLength() const
	{
	Vector2 xy(x, y);
	Vector2 dz(xy.octagonEstimateLength(), z);
	float result = dz.octagonEstimateLength();
	return result;
	}

float Vector3::calcLengthSquared() const
	{
	return x*x + y*y + z*z;
	}

float Vector3::calcHeadingRadians(Axis3 whichaxis) const
	{
	if (whichaxis == X_AXIS_3D)
		return atan2(z, y);
	if (whichaxis == Y_AXIS_3D)
		return atan2(z, x);
	if (whichaxis == Z_AXIS_3D)
		return atan2(y, x);
	return 0.f;
	}

float Vector3::calcHeadingRadiansCustomAxis(const Vector3& axis)
	{
	Vector3 right,up,fwd;
	getRightUpFwd(axis,right,up,fwd);
	float u=dotProduct(right);
	float v=dotProduct(up);
	float result=atan2(v,u);
	return result;
	}

float Vector3::calcHeadingDegreesCustomAxis(const Vector3& axis)
	{
	float result=ConvertRadiansToDegrees(calcHeadingRadiansCustomAxis(axis));
	return result;
	}

void Vector3::setXYZ(float xnew, float ynew, float znew)
	{
	x = xnew;
	y = ynew;
	z = znew;
	}

void Vector3::rotate90Degrees(Axis3 whichaxis)
	{
	Vector3 vnew(*this);
	if (whichaxis == X_AXIS_3D)
		{
		vnew.y = -z;
		vnew.z = y;
		}
	if (whichaxis == Y_AXIS_3D)
		{
		vnew.x = -z;
		vnew.z = x;
		}
	if (whichaxis == Z_AXIS_3D)
		{
		vnew.x = -y;
		vnew.y = x;
		}
	*this = vnew;
	}

void Vector3::rotateNegative90Degrees(Axis3 whichaxis)
	{
	Vector3 vnew(*this);
	if (whichaxis == X_AXIS_3D)
		{
		vnew.y = z;
		vnew.z = -y;
		}
	if (whichaxis == Y_AXIS_3D)
		{
		vnew.x = z;
		vnew.z = -x;
		}
	if (whichaxis == Z_AXIS_3D)
		{
		vnew.x = y;
		vnew.y = -x;
		}
	*this = vnew;
	}

void Vector3::rotateDegreesAroundCustomAxis(float degrees, const Vector3& rotaxis)
	{
	rotateRadiansAroundCustomAxis(ConvertDegreesToRadians(degrees), rotaxis);
	}

void Vector3::getRightUpFwd(const Vector3& rotaxis, Vector3& right, Vector3& up, Vector3& fwd)
	{
	Vector3 tmpRotAxis(rotaxis);
	fwd = tmpRotAxis.getNormalized();
	Vector3 worldUp(0.f,0.f,1.f);
	if (worldUp==fwd)
		{
		Vector3 worldRight(1.f,0.f,0.f);
		up=worldRight.crossProduct(fwd);
		right=fwd.crossProduct(up);
		}
	else
		{
		right = fwd.crossProduct(worldUp);
		up = right.crossProduct(fwd);
		}
	if(right.calcLengthSquared()!=1.f)
		right.normalize();
	if(up.calcLengthSquared()!=1.f)
		up.normalize();
	if(fwd.calcLengthSquared()!=1.f)
		fwd.normalize();
	}

void Vector3::rotateRadiansAroundCustomAxis(float radians, const Vector3& rotaxis)
	{
	Vector3 fwd, right, up;
	getRightUpFwd(rotaxis, right,up,fwd);
	Vector3 ruf = calcProjectionOntoRightUpFwd(right, up, fwd);
	Vector2 ru(ruf.x, ruf.y);
	ru.RotateRadians(radians);
	ruf.x = ru.x;
	ruf.y = ru.y;
	*this = right*ruf.x + up*ruf.y + fwd*ruf.z;
	}


float Vector3::normalize()
	{
	return setLength(1.f);
	}

void Vector3::rotateDegrees(float degrees, Axis3 whichaxis)
	{
	return rotateRadians(ConvertDegreesToRadians(degrees), whichaxis);
	}

void Vector3::rotateRadians(float radians, Axis3 whichaxis)
	{
	float *u, *v;
	switch (whichaxis)
		{
		case X_AXIS_3D:
			u = &y; v = &z; break;
		case Y_AXIS_3D:
			u = &z; v = &x; break;
		case Z_AXIS_3D:
		default:
			u = &x; v = &y; break;
		}
	Vector2 uv(*u, *v);
	uv.RotateRadians(radians);
	*u = uv.x;
	*v = uv.y;
	}

float Vector3::setLength(float newLength)
	{
	float oldLength = calcLength();
	if (oldLength == 0.f)
		return 0.f;
	float scale = newLength / oldLength;
	scaleUniform(scale);
	return oldLength;
	}

void Vector3::scaleUniform(float scale)
	{
	x *= scale;
	y *= scale;
	z *= scale;
	}

void Vector3::scaleNonUniform(const Vector3& perAxisScaleFactors)
	{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
	z *= perAxisScaleFactors.z;
	}

void Vector3::inverseScaleNonUniform(const Vector3& perAxisDivisors)
	{
	if (perAxisDivisors.x != 0.f)
		x /= perAxisDivisors.x;
	if (perAxisDivisors.y != 0.f)
		y /= perAxisDivisors.y;
	if (perAxisDivisors.z != 0.f)
		z /= perAxisDivisors.z;
	}

void Vector3::setHeadingDegrees(float pitchDegrees, float rollDegrees, float yawDegrees)
	{
	setHeadingRadians(ConvertDegreesToRadians(pitchDegrees),
		ConvertDegreesToRadians(rollDegrees),
		ConvertDegreesToRadians(yawDegrees));
	}

void Vector3::setHeadingRadians(float pitchRadians, float rollRadians, float yawRadians)
	{
	float length = calcLength();
	if (length == 0.f)
		return;
	Vector3 newvector = calcNormalizedVectorFromPitchRollYaw(pitchRadians, rollRadians, yawRadians);
	newvector *= length;
	*this = newvector;
	}

Vector3 Vector3::getNormalized()
	{
	Vector3 result(*this);
	result.normalize();
	return result;
	}

Vector3 Vector3::calcPortionOfLength(float length)
	{
	Vector3 result(*this);
	result.setLength(length);
	return result;
	}

Vector3 Vector3::getRandomNormalizedVector()
	{
	Vector3 result = getRandomVector();
	result.normalize();
	return result;
	}

Vector3 Vector3::getRandomVector(float length /*= 1.f*/)
	{
	Vector3 result;
	result.x = GetRandomFloatInRange(0.f, 100.f);
	result.y = GetRandomFloatInRange(0.f, 100.f);
	result.z = GetRandomFloatInRange(0.f, 100.f);
	result.setLength(length);
	return result;
	}

Vector3 Vector3::calcProjectionOntoRightUpFwd(const Vector3& right, const Vector3& up, const Vector3& fwd)
	{
	Vector3 result;
	Vector3 me(*this);
	result.x = me.dotProduct(right);
	result.y = me.dotProduct(up);
	result.z = me.dotProduct(fwd);
	return result;
	}

void Vector3::setUnitLengthAndHeadingDegrees(float pitchDegrees, float rollDegrees, float yawDegrees)
	{
	setHeadingDegrees(pitchDegrees, rollDegrees, yawDegrees);
	normalize();
	}

void Vector3::setUnitLengthAndHeadingRadians(float pitchRadians, float rollRadians, float yawRadians)
	{
	setHeadingRadians(pitchRadians, rollRadians, yawRadians);
	normalize();
	}

void Vector3::setLengthAndHeadingDegrees(float newLength, float pitchDegrees, float rollDegrees, float yawDegrees)
	{
	setHeadingDegrees(pitchDegrees, rollDegrees, yawDegrees);
	setLength(newLength);
	}

void Vector3::setLengthAndHeadingRadians(float newLength, float pitchRadians, float rollRadians, float yawRadians)
	{
	setHeadingRadians(pitchRadians, rollRadians, yawRadians);
	setLength(newLength);
	}

void Vector3::setLengthX(float length)
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

void Vector3::setLengthY(float length)
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

void Vector3::setLengthZ(float length)
	{
	if (z == 0.f)
		{
		z = length;
		}
	else
		{
		float scale = length / abs(z);
		z *= scale;
		}
	}

Vector3 Vector3::crossProduct(Vector3& other)
	{
	Vector3 result;
	result.x = y*other.z - z*other.y;
	result.y = z*other.x - x*other.z;
	result.z = x*other.y - y*other.x;
	return result;
	}

Vector3 Vector3::calcNormalizedVectorFromPitchRollYaw(float pitchRadians, float rollRadians, float yawRadians)
	{
	Vector3 result(1.f, 0.f, 0.f);
	result.rotateRadians(pitchRadians, X_AXIS_3D);
	result.rotateRadians(rollRadians, Z_AXIS_3D);
	result.rotateRadians(yawRadians, Y_AXIS_3D);
	result.normalize();
	return result;
	}

Vector3 Vector3::getVectorOfLength(float length)
	{
	Vector3 result(*this);
	result.setLength(length);
	return result;
	}

void  Vector3::operator*=(const Vector3& perAxisScaleFactors)
	{
	scaleNonUniform(perAxisScaleFactors);
	}

void Vector3::operator*=(float scale)
	{
	scaleUniform(scale);
	}

Vector3 Vector3::operator+(const Vector3& vectorToAdd) const
	{
	Vector3 result(x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z);
	return result;
	}

bool Vector3::operator!=(const Vector3& vectorToNotEqual) const
	{
	return !(*this == vectorToNotEqual);
	}

bool Vector3::operator==(const Vector3& vectorToEqual) const
	{
	bool result = x == vectorToEqual.x && y == vectorToEqual.y && z == vectorToEqual.z;
	return result;
	}

Vector3 Vector3::operator-(const Vector3& vectorToSubtract) const
	{
	Vector3 result(x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z);
	return result;
	}

Vector3 Vector3::operator*(float scale) const
	{
	Vector3 result(*this);
	result *= scale;
	return result;
	}

Vector3 Vector3::operator*(const Vector3& perAxisScaleFactors) const
	{
	Vector3 result(*this);
	result *= perAxisScaleFactors;
	return result;
	}

Vector3 Vector3::operator/(float inverseScale) const
	{
	assert(inverseScale != 0.f);
	float scale = 1.f / inverseScale;
	Vector3 result(*this);
	result *= scale;
	return result;
	}

Vector3 Vector3::operator/(const Vector3& vectorToDivide) const
	{
	Vector3 result(*this);
	assert(x != 0.f && y != 0.f && z != 0.f);
	result.z /= vectorToDivide.z;
	result.y /= vectorToDivide.y;
	result.x /= vectorToDivide.x;
	return result;
	}

void Vector3::operator+=(const Vector3& vectorToAdd)
	{
	x += vectorToAdd.x;
	z += vectorToAdd.z;
	y += vectorToAdd.y;
	}

void Vector3::operator-=(const Vector3& vectorToSubtract)
	{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
	z -= vectorToSubtract.z;
	}

Vector3 Vector3::operator-() const
	{
	Vector3 result(Vector3::ZERO - (*this));
	return result;
	}

void Vector3::operator/=(float inverseScale) 
	{
	assert(inverseScale != 0.f);
	float scale = 1.f / inverseScale;
	(*this) *= scale;
	}

void Vector3::operator/=(const Vector3& vectorToDivide) 
	{
	assert(vectorToDivide.x != 0.f && vectorToDivide.y != 0.f && vectorToDivide.z != 0.f);
	Vector3 result;
	x /= vectorToDivide.x;
	y /= vectorToDivide.y;
	z /= vectorToDivide.z;
	}

float calcDistance(const Vector3& positionA, const Vector3& positionB)
	{
	float result=sqrt(calcDistanceSquared(positionA,positionB));
	return result;
	}

float calcDistanceSquared(const Vector3& posA, const Vector3& posB)
	{
	Vector3 displacement = posB - posA;
	float result = displacement.calcLengthSquared();
	return result;
	}

bool isMostlyEqual(const Vector3& a, const Vector3& b)
	{
	bool result =  IsMostlyEqual(a.x, b.x)
				&& IsMostlyEqual(a.y, b.y)
				&& IsMostlyEqual(a.z, b.z);
	return result;
	}

float Vector3::dotProduct(const Vector3& other) const
	{
	return x*other.x + y*other.y + z*other.z;
	}

std::string Vector3::asString() const
	{
	std::stringstream ss;
	ss.precision(4);
	ss.str("");
	ss<<"("<<x<<","<<y<<","<<z<<")";
	return ss.str();
	}

float dotProduct(const Vector3& a, const Vector3& b)
	{
	return a.dotProduct(b);
	}

Vector3 operator * (float scale, const Vector3& vectorToScale)
	{
	Vector3 result(vectorToScale*scale);
	return result;
	}

Vector3 interpolate(float scaleA, const Vector3& a, float scaleB, const Vector3& b)
	{
	Vector3 result=a*scaleA+b*scaleB;
	return result;
	}

Vector2 Vector3::getAngleDegrees()
	{
	float aRadians=atan2(y,x);
	Vector2 hvec(x,y);
	float hlen=hvec.calcLength();
	float bRadians=atan2(z,hlen);
	Vector2 result(aRadians,bRadians);
	return result;
	}