#pragma once
#include <string>

class Vector3;

enum Axis4
	{
	X_AXIS_4D,
	Y_AXIS_4D,
	Z_AXIS_4D,
	W_AXIS_4D
	};

class Vector4
	{
	public: 
		Vector4();
		Vector4(float xset,float yset,float zset,float wset);
		Vector4(const Vector4& copy);
		Vector4(const Vector3& xyz,float wset);

		//-- new V3 methods --
		void getXYZW(float& xout,float& yout,float& zout, float& wout) const;
		const float* getAsFloatArray() const;
		float* getAsFloatArray();
		float calcLength3D() const;
		float calcLength4D() const;
		float octagonEstimateLength3D() const;
		float octagonEstimateLength4D() const;
		float calcLengthSquared3D() const;
		float calcLengthSquared4D() const;
		float calcHeadingRadians3D(Axis4 whichaxis) const;
		float calcHeadingRadiansCustomAxis3D(const Vector4& axis);
		float calcHeadingDegreesCustomAxis3D(const Vector4& axis);
		void setXYZW(float xnew,float ynew,float znew,float wnew);
		void setXYZ(const Vector3& vec3);
		void rotateDegreesAroundCustomAxis3D(float degrees,const Vector4& rotaxis);
		void rotateRadiansAroundCustomAxis3D(float radians,const Vector4& rotaxis);
		float normalize3D();
		float normalize4D();
		void rotateDegrees3D(float degrees,Axis4 whichaxis);
		void rotateRadians3D(float radians,Axis4 whichaxis);
		float setLength3D(float newLength);
		float setLength4D(float newLength);
		void scaleUniform3D(float scale);
		void scaleUniform4D(float scale);
		void scaleNonUniform(const Vector4& perAxisScaleFactors);
		void inverseScaleNonUniform(const Vector4& perAxisDivisors);
		void setHeadingDegrees3D(float pitchDegrees,float rollDegrees,float yawDegrees);
		void setHeadingRadians3D(float pitchRadians,float rollRadians,float yawRadians);
		Vector4 getNormalized3D();
		Vector4 getNormalized4D();
		Vector4 calcPortionOfLength3D(float length);
		Vector4 calcPortionOfLength4D(float length);
		static Vector4 getRandomNormalizedVector3D();
		static Vector4 getRandomNormalizedVector4D();
		static Vector4 getRandomVector3D(float length = 1.f);
		static Vector4 getRandomVector4D(float length = 1.f);
		void getRightUpFwd3D(const Vector4& rotaxis,Vector4& right,Vector4& up,Vector4& fwd);
		Vector4 calcProjectionOntoRightUpFwd3D(const Vector4& right,const Vector4& up,const Vector4& fwd);
		//TODO: Finish Vector4 implementation following this line
		void setUnitLengthAndHeadingDegrees3D(float pitchDegrees,float rollDegrees,float yawDegrees);
		void setUnitLengthAndHeadingRadians3D(float pitchRadians,float rollRadians,float yawRadians);
		void setLengthAndHeadingDegrees3D(float newLength,float pitchDegrees,float rollDegrees,float yawDegrees);
		void setLengthAndHeadingRadians3D(float newLength,float pitchRadians,float rollRadians,float yawRadians);
		float dotProduct(const Vector4& other) const;
		Vector4 crossProduct3D(Vector4& other);
		static Vector4 calcNormalizedVectorFromPitchRollYaw3D(float pitchRadians,float rollRadians,float yawRadians);
		bool doesHaveNoZeroDimensions() const;

		Vector4 operator+(const Vector4& other);
		Vector4 operator-(const Vector4& other);
		Vector4 operator*(float scale);
		void operator+=(const Vector4& other);
		void operator-=(const Vector4& other);
		void operator*=(float scale);

		const float& operator[](int index);

		friend float dotProduct(const Vector4& a, const Vector4& b);

		std::string asString();

	public: 
		float x;
		float y;
		float z;
		float w;
	public:
	};

