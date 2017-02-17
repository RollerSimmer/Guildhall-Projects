#pragma once
#include <string>

enum Axis3
	{
	X_AXIS_3D,
	Y_AXIS_3D,
	Z_AXIS_3D
	};

class Vector2;
class Vector4;
struct IntVector3;

class Vector3
	{
	public:
		static const Vector3 X_AXIS_VECTOR;
		static const Vector3 Y_AXIS_VECTOR;
		static const Vector3 Z_AXIS_VECTOR;
		static const Vector3 ZERO;
	public:
		explicit Vector3(float setx,float sety,float setz);
		Vector3(const Vector3& copy);
		Vector3();
		explicit Vector3(const Vector2& xy,float setz =0.f);
		explicit Vector3(const Vector4& truncVector);
		explicit Vector3(const IntVector3& intVector);
	public:
		//-- new V3 methods --
		void getXYZ(float& xout, float& yout, float& zout) const;
		const float* getAsFloatArray() const;
		float* getAsFloatArray();
		float calcLength() const;
		float octagonEstimateLength() const;
		float calcLengthSquared() const;
		float calcHeadingRadians(Axis3 whichaxis) const;
		float calcHeadingRadiansCustomAxis(const Vector3& axis);
		float calcHeadingDegreesCustomAxis(const Vector3& axis);
		void setXYZ(float xnew, float ynew, float znew);
		void rotate90Degrees(Axis3 whichaxis);
		void rotateNegative90Degrees(Axis3 whichaxis);
		void rotateDegreesAroundCustomAxis(float degrees,const Vector3& rotaxis);
		void rotateRadiansAroundCustomAxis(float radians,const Vector3& rotaxis);
		float normalize();
		void rotateDegrees(float degrees,Axis3 whichaxis);
		void rotateRadians(float radians,Axis3 whichaxis);
		float setLength(float newLength);
		void scaleUniform(float scale);
		void scaleNonUniform(const Vector3& perAxisScaleFactors);
		void inverseScaleNonUniform(const Vector3& perAxisDivisors);
		void setHeadingDegrees(float pitchDegrees, float rollDegrees, float yawDegrees);
		void setHeadingRadians(float pitchRadians, float rollRadians, float yawRadians);
		Vector3 getNormalized();
		Vector3 calcPortionOfLength(float length);
		static Vector3 getRandomNormalizedVector();
		static Vector3 getRandomVector(float length = 1.f);
		void getRightUpFwd(const Vector3& rotaxis, Vector3& right, Vector3& up, Vector3& fwd);
		Vector3 calcProjectionOntoRightUpFwd(const Vector3& right, const Vector3& up, const Vector3& fwd);
		//TODO: Finish Vector3 implementation following this line
		void setUnitLengthAndHeadingDegrees(float pitchDegrees, float rollDegrees, float yawDegrees);
		void setUnitLengthAndHeadingRadians(float pitchRadians, float rollRadians, float yawRadians);
		void setLengthAndHeadingDegrees(float newLength, float pitchDegrees, float rollDegrees, float yawDegrees);
		void setLengthAndHeadingRadians(float newLength, float pitchRadians, float rollRadians, float yawRadians);
		void setLengthX(float length);
		void setLengthY(float length);
		void setLengthZ(float length);
		float dotProduct(const Vector3& other) const;
		Vector3 crossProduct(Vector3& other);
		static Vector3 calcNormalizedVectorFromPitchRollYaw(float pitchRadians,float rollRadians,float yawRadians);
		Vector3 getVectorOfLength(float length);
		Vector2 getAngleDegrees();

	public:
		bool operator == (const Vector3& vectorToEqual) const;
		bool operator != (const Vector3& vectorToNotEqual) const;
		Vector3 operator + (const Vector3& vectorToAdd) const;
		Vector3 operator - (const Vector3& vectorToSubtract) const;
		Vector3 operator * (float scale) const;
		Vector3 operator * (const Vector3& perAxisScaleFactors) const;
		void operator *= (float scale);
		void operator *= (const Vector3& perAxisScaleFactors);
		Vector3 operator / (float inverseScale) const;
		Vector3 operator / (const Vector3& vectorToDivide) const;
		void operator /= (float inverseScale);
		void operator /= (const Vector3& vectorToDivide);
		void operator += (const Vector3& vectorToAdd);
		void operator -= (const Vector3& vectorToSubtract);
		Vector3 operator -() const;

	public:
		friend float calcDistance(const Vector3& positionA, const Vector3& positionB);
		friend float calcDistanceSquared(const Vector3& posA, const Vector3& posB);
		friend Vector3 operator * (float scale, const Vector3& vectorToScale);
		friend bool isMostlyEqual(const Vector3& a, const Vector3& b);
		friend float dotProduct(const Vector3& a, const Vector3& b);
		friend Vector3 interpolate(float scaleA,const Vector3& a,float scaleB,const Vector3& b);
		std::string asString() const;

	public:
		float x;
		float y;
		float z;
	private:
	public:
	};

