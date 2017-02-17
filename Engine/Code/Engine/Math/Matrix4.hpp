#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

const int AMT_MATRIX_4_CELLS = 4*4; //16
const int AMT_MATRIX_4_ROWS_OR_COLUMNS = 4;

enum MatrixDimension
	{
	MATRIX_DIM_X=0,
	MATRIX_DIM_Y=1,
	MATRIX_DIM_Z=2,
	MATRIX_DIM_W=3
	};

class Matrix4
	{
	public: 
		Matrix4(const Matrix4& copy);
		Matrix4();
		explicit Matrix4(const float* arrayOfFloats);
		explicit Matrix4(const Vector2& iBasis,const Vector2& jBasis,const Vector2& translation=Vector2(0.f,0.f));
		explicit Matrix4(const Vector3& iBasis,const Vector3& jBasis,const Vector3& kBasis,const Vector3& translation=Vector3(0.f,0.f,0.f));
		explicit Matrix4(const Vector4& iBasis,const Vector4& jBasis,const Vector4& kBasis,const Vector4& translation=Vector4(0.f,0.f,0.f,1.f));

		void setCell(int j,int i,float cellValue);
		float getCell(int j,int i) const;

		void setToZeroMatrix();
		void makeIdentity();

		const float* getAsFloatArray() const;	// Used when the Matrix4 in question in const
		float* 	     getAsFloatArray();		// Used when the Matrix4 is not const

		void concatenateTransform(const Matrix4& other);
		const Matrix4 getTransformed(const Matrix4& other);
		Vector2 transformPosition(const Vector2& position2D) const; // Assumes z=0, w=1
		Vector3 transformPosition(const Vector3& position3D) const; // Assumes w=1
		Vector2 transformDirection(const Vector2& direction2D) const; // Assumes z=0, w=0
		Vector3 transformDirection(const Vector3& direction3D) const; // Assumes w=0
		Vector4 transformVector(const Vector4& homogeneousVector) const; // w is provided

		void translate(const Vector2& translation2D); // z translation assumed to be 0
		void translate(const Vector3& translation3D);

		void scale(float uniformScale);
		void scale(const Vector2& nonUniformScale2D); // z scale assumed to be 1
		void scale(const Vector3& nonUniformScale3D);

		void rotateDegreesAboutX(float degrees);
		void rotateDegreesAboutY(float degrees);
		void rotateDegreesAboutZ(float degrees); // Possibly also offered as RotateDegrees2D
		void rotateRadiansAboutX(float radians);
		void rotateRadiansAboutY(float radians);
		void rotateRadiansAboutZ(float radians);

		static Matrix4 createTranslation(const Vector2& translation2D); // z translation assumed to be 0
		static Matrix4 createTranslation(const Vector3& translation3D);
		static Matrix4 createScale(float uniformScale);
		static Matrix4 createScale(const Vector2& nonUniformScale2D); // z scale assumed to be 1
		static Matrix4 createScale(const Vector3& nonUniformScale3D);
		static Matrix4 createRotationDegreesAboutX(float degrees);
		static Matrix4 createRotationDegreesAboutY(float degrees);
		static Matrix4 createRotationDegreesAboutZ(float degrees); // a.k.a. CreateRotationDegrees2D
		static Matrix4 createRotationRadiansAboutX(float radians);
		static Matrix4 createRotationRadiansAboutY(float radians);
		static Matrix4 createRotationRadiansAboutZ(float radians); // a.k.a. CreateRotationRadians2D

		static Matrix4 calcQuaternionRotationMatrix(const Vector3& rotAxis, const Vector3& translation, float angleDegrees);

		Matrix4 getTransposed() const;

		std::string asString();

	private:
		void rotateRadiansAboutAxis(float radians, MatrixDimension dimA, MatrixDimension dimB);
// 		void operator = (const Matrix4& copy);

	public:
		union 
			{
			float m_values[AMT_MATRIX_4_CELLS];
			Vector4 rows[AMT_MATRIX_4_ROWS_OR_COLUMNS];
			struct { Vector4 i,j,k,t; };
			};
		static Matrix4 makeOrthoProjection(float left, float right, float bottom, float top, float nearZ, float farZ);
	};

