#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cmath>
#include <cassert>
#include <sstream>

Vector4::Vector4(float xset,float yset,float zset,float wset)
	: x(xset)
	, y(yset)
	, z(zset)
	, w(wset)
	{

	}

Vector4::Vector4(const Vector4& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(copy.w)
	{

	}

Vector4::Vector4(const Vector3& xyz,float wset)
	: x(xyz.x)
	, y(xyz.y)
	, z(xyz.z)
	, w(wset)
	{

	}

Vector4::Vector4()
	: x(0.f)
	, y(0.f)
	, z(0.f)
	, w(0.f)
	{

	}

void Vector4::getXYZW(float& xout,float& yout,float& zout,float& wout) const
	{
	xout=x;
	yout=y;
	zout=z;
	wout=w;
	}

const float* Vector4::getAsFloatArray() const
	{
	return &x;
	}

float* Vector4::getAsFloatArray()
	{
	return &x;
	}

float Vector4::calcLength3D() const
	{
	return sqrt(calcLengthSquared3D());
	}

float Vector4::calcLength4D() const
	{
	return sqrt(calcLengthSquared4D());
	}

float Vector4::octagonEstimateLength3D() const
	{
	Vector3 v3(x,y,z);
	float result=v3.octagonEstimateLength();
	return result;
	}

float Vector4::octagonEstimateLength4D() const
	{
	Vector2 v2(octagonEstimateLength3D(),w);
	float result=v2.octagonEstimateLength();
	return result;
	}

float Vector4::calcLengthSquared3D() const
	{
	float result = x*x + y*y + z*z;
	return result;
	}

float Vector4::calcLengthSquared4D() const
	{
	float result = calcLengthSquared3D() + w*w;
	return result;
	}

float Vector4::calcHeadingRadians3D(Axis4 whichaxis) const
	{
	Vector3 this3d(x,y,z);
	assert(whichaxis!=W_AXIS_4D);
	Axis3 whichAxis3d=(Axis3)whichaxis;
	return this3d.calcHeadingRadians(whichAxis3d);
	}

float Vector4::calcHeadingRadiansCustomAxis3D(const Vector4& axis)
	{
	Vector3 this3d(x,y,z);
	Vector3 axis3d(axis.x,axis.y,axis.z);
	float result=this3d.calcHeadingRadiansCustomAxis(axis3d);
	return result;
	}

float Vector4::calcHeadingDegreesCustomAxis3D(const Vector4& axis)
	{
	float result=ConvertRadiansToDegrees(calcHeadingRadiansCustomAxis3D(axis));
	return result;
	}

void Vector4::setXYZW(float xnew,float ynew,float znew,float wnew)
	{
	x=xnew;
	y=ynew;
	z=znew;
	w=wnew;
	}

void Vector4::setXYZ(const Vector3& vec3)
	{
	x=vec3.x;
	y=vec3.y;
	z=vec3.z;
	}

void Vector4::rotateDegreesAroundCustomAxis3D(float degrees,const Vector4& rotaxis)
	{
	rotateRadiansAroundCustomAxis3D(ConvertDegreesToRadians(degrees),rotaxis);
	}

void Vector4::rotateRadiansAroundCustomAxis3D(float radians,const Vector4& rotaxis)
	{
	Vector3 this3d(x,y,z);
	Vector3 axis3d(rotaxis.x,rotaxis.y,rotaxis.z);
	this3d.rotateRadiansAroundCustomAxis(radians,axis3d);
	setXYZ(this3d);
	}

float Vector4::normalize3D()
	{
	float len=calcLength3D();
	if(len==0.f)
		return len;
	float invlen=1.f/len;
	scaleUniform3D(invlen);
	return len;
	}

float Vector4::normalize4D()
	{
	float len=calcLength4D();
	if(len==0.f)
		return len;
	float invlen=1.f/len;
	scaleUniform4D(invlen);
	return len;
	}

void Vector4::rotateDegrees3D(float degrees,Axis4 whichaxis)
	{
	rotateRadians3D(ConvertDegreesToRadians(degrees),whichaxis);
	}

void Vector4::rotateRadians3D(float radians,Axis4 whichaxis)
	{
	Vector3 this3d(x,y,z);
	assert(whichaxis!=W_AXIS_4D);
	Axis3 whichaxis3d=(Axis3)whichaxis;
	this3d.rotateRadians(radians,whichaxis3d);
	setXYZ(this3d);
	}

float Vector4::setLength3D(float newLength)
	{
	float curlen=calcLength3D();
	if(curlen==0)
		return curlen;
	float scale=newLength/curlen;
	scaleUniform3D(scale);
	return curlen;
	}

float Vector4::setLength4D(float newLength)
	{
	float curlen=calcLength4D();
	if(curlen==0)
		return curlen;
	float scale=newLength/curlen;
	scaleUniform4D(scale);
	return curlen;
	}

void Vector4::scaleUniform3D(float scale)
	{
	x*=scale;
	y*=scale;
	z*=scale;
	}

void Vector4::scaleUniform4D(float scale)
	{
	scaleUniform3D(scale);
	w*=scale;
	}

void Vector4::scaleNonUniform(const Vector4& perAxisScaleFactors)
	{
	x*=perAxisScaleFactors.x;
	y*=perAxisScaleFactors.y;
	z*=perAxisScaleFactors.z;
	w*=perAxisScaleFactors.w;
	}

void Vector4::inverseScaleNonUniform(const Vector4& perAxisDivisors)
	{
	assert(perAxisDivisors.doesHaveNoZeroDimensions());
	x/=perAxisDivisors.x;
	y/=perAxisDivisors.y;
	z/=perAxisDivisors.z;
	w/=perAxisDivisors.w;
	}

void Vector4::setHeadingDegrees3D(float pitchDegrees,float rollDegrees,float yawDegrees)
	{
	float pitchRadians=ConvertDegreesToRadians(pitchDegrees);
	float rollRadians=ConvertDegreesToRadians(rollDegrees);
	float yawRadians=ConvertDegreesToRadians(yawDegrees);
	setHeadingRadians3D(pitchRadians,rollRadians,yawRadians);
	}

void Vector4::setHeadingRadians3D(float pitchRadians,float rollRadians,float yawRadians)
	{
	Vector3 this3d(x,y,z);
	this3d.setHeadingRadians(pitchRadians,rollRadians,yawRadians);
	setXYZ(this3d);
	}

Vector4 Vector4::getNormalized3D()
	{
	Vector4 result(*this);
	result.normalize3D();
	return result;
	}

Vector4 Vector4::getNormalized4D()
	{
	Vector4 result(*this);
	result.normalize4D();
	return result;
	}

Vector4 Vector4::calcPortionOfLength3D(float length)
	{
	Vector4 result(*this);
	result.setLength3D(length);
	return result;
	}

Vector4 Vector4::calcPortionOfLength4D(float length)
	{
	Vector4 result(*this);
	result.setLength4D(length);
	return result;
	}

Vector4 Vector4::getRandomNormalizedVector3D()
	{
	Vector4 result=getRandomVector3D();
	//result.normalize3D();
	return result;
	}

Vector4 Vector4::getRandomNormalizedVector4D()
	{
	Vector4 result=getRandomVector4D();
	//result.normalize4D();
	return result;
	}

Vector4 Vector4::getRandomVector3D(float length /*= 1.f*/)
	{
	float xrand=GetRandomFloatInRange(0.f,1.f);
	float yrand=GetRandomFloatInRange(0.f,1.f);
	float zrand=GetRandomFloatInRange(0.f,1.f);
	Vector4 result(xrand,yrand,zrand,1.f);
	result.setLength3D(length);
	return result;
	}

Vector4 Vector4::getRandomVector4D(float length /*= 1.f*/)
	{
	float xrand=GetRandomFloatInRange(0.f,1.f);
	float yrand=GetRandomFloatInRange(0.f,1.f);
	float zrand=GetRandomFloatInRange(0.f,1.f);
	float wrand=GetRandomFloatInRange(0.f,1.f);
	Vector4 result(xrand,yrand,zrand,wrand);
	result.setLength4D(length);
	return result;
	}

void Vector4::getRightUpFwd3D(const Vector4& rotaxis,Vector4& right,Vector4& up,Vector4& fwd)
	{
	Vector3 this3d(x,y,z);
	Vector3 right3d;
	Vector3 up3d;
	Vector3 fwd3d;
	Vector3 rotAxis3d(rotaxis.x,rotaxis.y,rotaxis.z);
	this3d.getRightUpFwd(rotAxis3d,right3d,up3d,fwd3d);
	right=Vector4(right3d,w);
	up=Vector4(up3d,w);
	fwd=Vector4(fwd3d,w);
	}

Vector4 Vector4::calcProjectionOntoRightUpFwd3D(const Vector4& right,const Vector4& up,const Vector4& fwd)
	{
	Vector3 this3d(x,y,z);
	Vector3 right3d(right.x,right.y,right.z);
	Vector3 up3d(up.x,up.y,up.z);
	Vector3 fwd3d(fwd.x,fwd.y,fwd.z);
	Vector3 result3d=this3d.calcProjectionOntoRightUpFwd(right3d,up3d,fwd3d);
	Vector4 result=Vector4(result3d,w);
	return result;
	}

void Vector4::setUnitLengthAndHeadingDegrees3D(float pitchDegrees,float rollDegrees,float yawDegrees)
	{
	setLengthAndHeadingDegrees3D(1.f,pitchDegrees,rollDegrees,yawDegrees);
	}

void Vector4::setUnitLengthAndHeadingRadians3D(float pitchRadians,float rollRadians,float yawRadians)
	{
	setLengthAndHeadingRadians3D(1.f,pitchRadians,rollRadians,yawRadians);
	}

void Vector4::setLengthAndHeadingDegrees3D(float newLength,float pitchDegrees,float rollDegrees,float yawDegrees)
	{
	setHeadingDegrees3D(pitchDegrees,rollDegrees,yawDegrees);
	setLength3D(newLength);
	}

void Vector4::setLengthAndHeadingRadians3D(float newLength,float pitchRadians,float rollRadians,float yawRadians)
	{
	setHeadingRadians3D(pitchRadians,rollRadians,yawRadians);
	setLength3D(newLength);
	}

const float& Vector4::operator[](int index)
	{
	assert(index>=0 && index<=3);
	float* v=&x;
	return v[index];
	}

float Vector4::dotProduct(const Vector4& other) const
	{
	float result = x*other.x
		         + y*other.y
		         + z*other.z
		         + w*other.w;
	return result;
	}

std::string Vector4::asString()
	{
	std::stringstream ss;
	ss.str("");
	ss<<"("<<x<<","<<y<<","<<z<<","<<w<<")";
	return ss.str();
	}

Vector4 Vector4::crossProduct3D(Vector4& other)
	{
	Vector3 this3d(x,y,z);
	Vector3 other3d(other.x,other.y,other.z);
	Vector3 result3d=this3d.crossProduct(other3d);
	Vector4 result(result3d,w);
	return result;
	}

Vector4 Vector4::calcNormalizedVectorFromPitchRollYaw3D(float pitchRadians,float rollRadians,float yawRadians)
	{
	Vector3 result3d=Vector3::calcNormalizedVectorFromPitchRollYaw(pitchRadians,rollRadians,yawRadians);
	Vector4 result(result3d,1.f);
	return result;
	}

bool Vector4::doesHaveNoZeroDimensions() const
	{
	return !(x==0||y==0||z==0||w==0);
	}

Vector4 Vector4::operator+(const Vector4& other)
	{
	Vector4 result=*this;
	result+=other;
	return result;
	}

Vector4 Vector4::operator-(const Vector4& other)
	{
	Vector4 result=*this;
	result-=other;
	return result;
	}

Vector4 Vector4::operator*(float scale)
	{
	Vector4 result=*this;
	result*=scale;
	return result;
	}

void Vector4::operator-=(const Vector4& other)
	{
	x-=other.x;
	y-=other.y;
	z-=other.z;
	w-=other.w;
	}

void Vector4::operator+=(const Vector4& other)
	{
	x+=other.x;
	y+=other.y;
	z+=other.z;
	w+=other.w;
	}

void Vector4::operator*=(float scale)
	{
	x*=scale;
	y*=scale;
	z*=scale;
	w*=scale;
	}

float dotProduct(const Vector4& a,const Vector4& b)
	{
	return a.dotProduct(b);
	}