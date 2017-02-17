#pragma once

enum Axis2
	{
	X_AXIS_2D,
	Y_AXIS_2D,
	AMT_2D_AXES
	};

class Vector3;
class Vector4;

class Vector2 
	{
	public:
		static const Vector2 ZERO;
		static const Vector2 ONE;
	public:
		float x;
		float y;
	public: 
		Vector2();			
		Vector2(const Vector2& copy);
		explicit Vector2(float initialX,float initialY);
		explicit Vector2(const Vector3& truncVector);
		explicit Vector2(const Vector4& truncVector);

	public:
		void GetXY( float& out_x, float& out_y ) const;
		const float* GetAsFloatArray() const;
		float* GetAsFloatArray();
		float calcLength() const;
		float octagonEstimateLength() const;
		float calcLengthSquared() const;
		float calcHeadingDegrees() const;
		float CalcHeadingRadians() const;
		void SetXY(float newX, float newY);
		void Rotate90Degrees();
		void RotateNegative90Degrees();
		float Normalize();
		void RotateDegrees(float degrees);
		void RotateRadians(float radians);
		float setLength(float newLength);
		void ScaleUniform(float scale);
		void ScaleNonUniform(const Vector2& perAxisScaleFactors);
		void InverseScaleNonUniform(const Vector2& perAxisDivisors);
		void SetHeadingDegrees(float headingDegrees);
		void SetHeadingRadians(float headingRadians);
		Vector2 getNormalized();	
		Vector2 calcPortionOfLength(float length);
		static Vector2 getRandomNormalizedVector();
		static Vector2 getRandomVector(float length=1.f);

		void SetUnitLengthAndHeadingDegrees(float headingDegrees);
		void SetUnitLengthAndHeadingRadians(float headingRadians);
		void SetLengthAndHeadingDegrees(float newLength, float headingDegrees);
		void SetLengthAndHeadingRadians(float newLength, float headingRadians);
		void setLengthX(float length);
		void setLengthY(float length);

		float dotProduct(const Vector2& other) const;

	public:
		bool operator == ( const Vector2& vectorToEqual ) const;
		bool operator != ( const Vector2& vectorToNotEqual ) const;
		const Vector2 operator + ( const Vector2& vectorToAdd ) const;
		const Vector2 operator - ( const Vector2& vectorToSubtract ) const;
		const Vector2 operator * (float scale) const;
		const Vector2 operator * ( const Vector2& perAxisScaleFactors ) const;
		const Vector2& operator *= (float scale) ;
		const Vector2& operator *= (const Vector2& perAxisScaleFactors) ;
		const Vector2 operator / (float inverseScale) const;
		const Vector2 operator / (const Vector2& vectorToDivide) const;
		void operator += ( const Vector2& vectorToAdd );
		void operator -= ( const Vector2& vectorToSubtract );
		Vector2 operator -() const;
	public:
		friend float CalcDistance( const Vector2& positionA, const Vector2& positionB );
		friend float CalcDistanceSquared( const Vector2& posA, const Vector2& posB );
		friend const Vector2 operator * ( float scale, const Vector2& vectorToScale );
		friend bool IsMostlyEqual(const Vector2& a, const Vector2& b);
		friend float dotProduct(const Vector2& a, const Vector2& b);
		friend Vector2 makeVector2(float vx, float vy);
		friend Vector2 interpolate(float scaleA, const Vector2& a, float scaleB, const Vector2& b);
	};
