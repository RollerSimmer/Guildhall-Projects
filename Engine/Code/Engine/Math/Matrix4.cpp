#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <cstring>
#include <sstream>

Matrix4::Matrix4(const Matrix4& copy)
	{
	for (int icell = 0; icell < AMT_MATRIX_4_CELLS ; icell++)
		{
		m_values[icell]=copy.m_values[icell];
		}
	}

Matrix4::Matrix4(const float* arrayOfFloats)
	{
	for (int icell = 0; icell < AMT_MATRIX_4_CELLS ; icell++)
		{
		m_values[icell]=arrayOfFloats[icell];
		}
	}

Matrix4::Matrix4(const Vector2& iBasis,const Vector2& jBasis,const Vector2& translation/*=Vector2(0.f,0.f)*/)
	{
	makeIdentity();
	memcpy(&m_values[0],&iBasis,sizeof(iBasis));
	memcpy(&m_values[4],&jBasis,sizeof(jBasis));
	memcpy(&m_values[12],&translation,sizeof(translation));
	}

Matrix4::Matrix4(const Vector3& iBasis,const Vector3& jBasis,const Vector3& kBasis,const Vector3& translation/*=Vector3(0.f,0.f,0.f)*/)
	{
	makeIdentity();
	memcpy(&m_values[0],&iBasis,sizeof(iBasis));
	memcpy(&m_values[4],&jBasis,sizeof(jBasis));
	memcpy(&m_values[8],&kBasis,sizeof(kBasis));
	memcpy(&m_values[12],&translation,sizeof(translation));
	}

Matrix4::Matrix4(const Vector4& iBasis,const Vector4& jBasis,const Vector4& kBasis,const Vector4& translation/*=Vector4(0.f,0.f,0.f,1.f)*/)
	{
	makeIdentity();
	memcpy(&m_values[0],&iBasis,sizeof(iBasis));
	memcpy(&m_values[4],&jBasis,sizeof(jBasis));
	memcpy(&m_values[8],&kBasis,sizeof(kBasis));
	memcpy(&m_values[12],&translation,sizeof(translation));
	}

Matrix4::Matrix4()
	{
	makeIdentity();
	}

void Matrix4::setCell(int j,int i,float cellValue)
	{
	i=i&0x3;
	j=j&0x3;
	m_values[(j<<2) | i] = cellValue;
	}

float Matrix4::getCell(int j,int i) const
	{
	i=i&0x3;
	j=j&0x3;
	return m_values[(j<<2) | i];
	}

void Matrix4::setToZeroMatrix()
	{
	for (int icell = 0; icell < AMT_MATRIX_4_CELLS ; icell++)
		{
		m_values[icell]=0.f;
		}
	}

void Matrix4::makeIdentity()
	{
	for(int i=0;i<16;i++)
		m_values[i]=0.f;
	for (int i=0;i<16;i+=5)
		m_values[i]=1.f;
	}

const float* Matrix4::getAsFloatArray() const
	{
	return &m_values[0];
	}

float* Matrix4::getAsFloatArray()
	{
	return &m_values[0];
	}

void Matrix4::concatenateTransform(const Matrix4& other)
	{
	*this=this->getTransformed(other);
	}

// const Matrix4 Matrix4::getTransformed(const Matrix4& other)
// 	{
// 	Vector4 leftCols[4]=
// 		{
// 		Vector4(m_values[0],m_values[4],m_values[ 8],m_values[12]),
// 		Vector4(m_values[1],m_values[5],m_values[ 9],m_values[13]),
// 		Vector4(m_values[2],m_values[6],m_values[10],m_values[14]),
// 		Vector4(m_values[3],m_values[7],m_values[11],m_values[15])
// 		};
// 	Vector4 rightRows[4]=
// 		{
// 		Vector4(other.m_values[ 0],other.m_values[ 1],other.m_values[ 2],other.m_values[ 3]),
// 		Vector4(other.m_values[ 4],other.m_values[ 5],other.m_values[ 6],other.m_values[ 7]),
// 		Vector4(other.m_values[ 8],other.m_values[ 9],other.m_values[10],other.m_values[11]),
// 		Vector4(other.m_values[12],other.m_values[13],other.m_values[14],other.m_values[15])
// 		};
// 
// 	Matrix4 result;
// 
// 	#define CELL_MUL(j,i) result.m_values[j*4+i]=leftCols[j].dotProduct(rightRows[i])
// 
// 	CELL_MUL( 0, 0);
// 	CELL_MUL( 0, 1);
// 	CELL_MUL( 0, 2);
// 	CELL_MUL( 0, 3);
// 	CELL_MUL( 1, 0);
// 	CELL_MUL( 1, 1);
// 	CELL_MUL( 1, 2);
// 	CELL_MUL( 1, 3);
// 	CELL_MUL( 2, 0);
// 	CELL_MUL( 2, 1);
// 	CELL_MUL( 2, 2);
// 	CELL_MUL( 2, 3);
// 	CELL_MUL( 3, 0);
// 	CELL_MUL( 3, 1);
// 	CELL_MUL( 3, 2);
// 	CELL_MUL( 3, 3);
// 
// 	return result;
// 	}

const Matrix4 Matrix4::getTransformed(const Matrix4& other)
	{
	Vector4 rightCols[4]=
		{
		Vector4(other.m_values[0],other.m_values[4],other.m_values[ 8],other.m_values[12]),
		Vector4(other.m_values[1],other.m_values[5],other.m_values[ 9],other.m_values[13]),
		Vector4(other.m_values[2],other.m_values[6],other.m_values[10],other.m_values[14]),
		Vector4(other.m_values[3],other.m_values[7],other.m_values[11],other.m_values[15])
		};
	Vector4 leftRows[4]=
		{
		Vector4(m_values[ 0],m_values[ 1],m_values[ 2],m_values[ 3]),
		Vector4(m_values[ 4],m_values[ 5],m_values[ 6],m_values[ 7]),
		Vector4(m_values[ 8],m_values[ 9],m_values[10],m_values[11]),
		Vector4(m_values[12],m_values[13],m_values[14],m_values[15])
		};

	Matrix4 result;

	#define CELL_MUL(j,i) result.m_values[j*4+i]=leftRows[j].dotProduct(rightCols[i])

	CELL_MUL(0,0);
	CELL_MUL(0,1);
	CELL_MUL(0,2);
	CELL_MUL(0,3);
	CELL_MUL(1,0);
	CELL_MUL(1,1);
	CELL_MUL(1,2);
	CELL_MUL(1,3);
	CELL_MUL(2,0);
	CELL_MUL(2,1);
	CELL_MUL(2,2);
	CELL_MUL(2,3);
	CELL_MUL(3,0);
	CELL_MUL(3,1);
	CELL_MUL(3,2);
	CELL_MUL(3,3);

	return result;
	}

Vector2 Matrix4::transformPosition(const Vector2& position2D) const
	{
	Vector3 result3d=transformPosition(Vector3(position2D,0.f));
	Vector2 result=Vector2(result3d);
	return result;
	}

Vector3 Matrix4::transformPosition(const Vector3& position3D) const
	{
	Vector4 dir4d(position3D,1.f);
	Vector4 result4d=transformVector(dir4d);
	Vector3 result(result4d.x,result4d.y,result4d.z);
	return result;
	}


Vector2 Matrix4::transformDirection(const Vector2& direction2D) const
	{
	Vector3 result3d=transformDirection(Vector3(direction2D,0.f));
	Vector2 result=Vector2(result3d);
	return result;
	}

Vector3 Matrix4::transformDirection(const Vector3& direction3D) const
	{
	Vector4 dir4d(direction3D,0.f);
	Vector4 result4d=transformVector(dir4d);
	Vector3 result(result4d.x,result4d.y,result4d.z);
	return result;
	}

Vector4 Matrix4::transformVector(const Vector4& homogeneousVector) const
	{
	const float* vecarray=homogeneousVector.getAsFloatArray();
	Vector4 result;
	float* resultArray=result.getAsFloatArray();
	for (int col=0;col<4;col++)
		{
		float sum=0.f;
		for (int row=0;row<4;row++)
			{
			float term=getCell(row,col)*vecarray[row];
			sum+=term;
			}
		resultArray[col]=sum;
		}
	return result;
	}

void Matrix4::translate(const Vector2& translation2D)
	{
	Vector3 translation3d(translation2D,0.f);
	translate(translation3d);
	}

void Matrix4::translate(const Vector3& translation3D)
	{
	Vector4 translation4D(translation3D,1.f);
	const float* translationArray = translation4D.getAsFloatArray();
	int col=3;
	for (int row=0;row<4;row++)
		{
		setCell(row,col,translationArray[row]);
		}
	}

void Matrix4::scale(float uniformScale)
	{
	setToZeroMatrix();
	for (int ij = 0; ij < 3 ; ij++)
		{
		setCell(ij, ij, uniformScale);
		}
	setCell(3,3,1.f);
	}

void Matrix4::scale(const Vector2& nonUniformScale2D)
	{
	scale(Vector3(nonUniformScale2D,1.f));
	}

void Matrix4::scale(const Vector3& nonUniformScale3D)
	{
	const float* scaleArray=nonUniformScale3D.getAsFloatArray();
	for (int ij = 0; ij < 3 ; ij++)
		{
		setCell(ij,ij,scaleArray[ij]);
		}
	setCell(3,3,1.f);
	}

void Matrix4::rotateDegreesAboutX(float degrees)
	{
	rotateRadiansAboutX(ConvertDegreesToRadians(degrees));
	}

void Matrix4::rotateDegreesAboutY(float degrees)
	{
	rotateRadiansAboutY(ConvertDegreesToRadians(degrees));
	}

void Matrix4::rotateDegreesAboutZ(float degrees)
	{
	rotateRadiansAboutZ(ConvertDegreesToRadians(degrees));
	}

void Matrix4::rotateRadiansAboutX(float radians)
	{
	rotateRadiansAboutAxis(radians,MATRIX_DIM_Y,MATRIX_DIM_Z);
	}

void Matrix4::rotateRadiansAboutY(float radians)
	{
	rotateRadiansAboutAxis(radians,MATRIX_DIM_Z,MATRIX_DIM_X);
	}

void Matrix4::rotateRadiansAboutZ(float radians)
	{
	rotateRadiansAboutAxis(radians,MATRIX_DIM_X,MATRIX_DIM_Y);
	}

Matrix4 Matrix4::createTranslation(const Vector2& translation2D)
	{
	Matrix4 result;
	result.translate(translation2D);
	return result;
	}

Matrix4 Matrix4::createTranslation(const Vector3& translation3D)
	{
	Matrix4 result;
	result.translate(translation3D);
	return result;
	}

Matrix4 Matrix4::createScale(float uniformScale)
	{
	Matrix4 result;
	result.scale(uniformScale);
	return result;
	}

Matrix4 Matrix4::createScale(const Vector2& nonUniformScale2D)
	{
	Matrix4 result;
	result.scale(nonUniformScale2D);
	return result;
	}

Matrix4 Matrix4::createScale(const Vector3& nonUniformScale3D)
	{
	Matrix4 result;
	result.scale(nonUniformScale3D);
	return result;
	}

Matrix4 Matrix4::createRotationDegreesAboutX(float degrees)
	{
	Matrix4 result;
	result.rotateDegreesAboutX(degrees);
	return result;
	}

Matrix4 Matrix4::createRotationDegreesAboutY(float degrees)
	{
	Matrix4 result;
	result.rotateDegreesAboutY(degrees);
	return result;
	}

Matrix4 Matrix4::createRotationDegreesAboutZ(float degrees)
	{
	Matrix4 result;
	result.rotateDegreesAboutZ(degrees);
	return result;
	}

Matrix4 Matrix4::createRotationRadiansAboutX(float radians)
	{
	Matrix4 result;
	result.rotateRadiansAboutX(radians);
	return result;
	}

Matrix4 Matrix4::createRotationRadiansAboutY(float radians)
	{
	Matrix4 result;
	result.rotateRadiansAboutY(radians);
	return result;
	}

Matrix4 Matrix4::createRotationRadiansAboutZ(float radians)
	{
	Matrix4 result;
	result.rotateRadiansAboutZ(radians);
	return result;
	}

// Matrix4 Matrix4::calcQuaternionRotationMatrix(const Vector3& rotAxis,const Vector3& translation,float angleDegrees)
// 	{
// 	Matrix4 result;
// 	float* M = result.getAsFloatArray(); //alias
// 
// 	Vector3 rotAxisVar(rotAxis);
// 	if (rotAxisVar.calcLengthSquared()!=1.f)
// 		rotAxisVar.normalize();
// 
//  float halfAngleDegrees=0.5f*angleDegrees;
// 	float cosa=cosDegrees(halfAngleDegrees);
// 	float sina=sinDegrees(halfAngleDegrees);
// 
// 	Vector4    q(cosa,sina*rotAxisVar.x,sina*rotAxisVar.y,sina*rotAxisVar.z);
// 
// 	float qiqr=q.y*q.x;
// 	float qjqr=q.z*q.x;
// 	float qkqr=q.w*q.x;
// 	float qi2=q.y*q.y;
// 	float qj2=q.z*q.z;
// 	float qk2=q.w*q.w;
// 	float qiqj=q.y*q.z;
// 	float qiqk=q.y*q.w;
// 	float qjqk=q.z*q.z;
// 
// 	int i=0;
// 
// 	M[i++]=1.f-2.f*(qj2+qk2);
// 	M[i++]=2.f*(qiqj+qkqr);
// 	M[i++]=2.f*(qiqk-qjqr);
// 	M[i++]=0.f;
// 	M[i++]=2.f*(qiqj-qkqr);
// 	M[i++]=1.f-2.f*(qi2+qk2);
// 	M[i++]=2.f*(qjqk+qiqr);
// 	M[i++]=0.f;
// 	M[i++]=2.f*(qiqk+qjqr);
// 	M[i++]=2.f*(qjqk-qiqr);
// 	M[i++]=1.f-2.f*(qi2+qj2);
// 	M[i++]=0.f;
// 	M[i++]=translation.x;
// 	M[i++]=translation.y;
// 	M[i++]=translation.z;
// 	M[i++]=1.f;
// 
// 	return result;
// 	}

Matrix4 Matrix4::calcQuaternionRotationMatrix(const Vector3& rotAxis,const Vector3& translation,float angleDegrees)
	{
	Matrix4 result;
	float* M = result.getAsFloatArray(); //alias

	Vector3 rotAxisVar(rotAxis);
	if (rotAxisVar.calcLengthSquared()!=1.f)
		rotAxisVar.normalize();

	float halfAngleDegrees=0.5f*angleDegrees;
	float cost=cosDegrees(halfAngleDegrees);
	float sint=sinDegrees(halfAngleDegrees);

	Vector4    q(cost,sint*rotAxisVar.x,sint*rotAxisVar.y,sint*rotAxisVar.z);

	float a=q.x;
	float b=q.y;
	float c=q.z;
	float d=q.w;
	float a2=a*a;
	float b2=b*b;
	float c2=c*c;
	float d2=d*d;
	float ab=a*b;
	float ac=a*c;
	float ad=a*d;
	float bc=b*c;
	float bd=b*d;
	float cd=c*d;

	int i=0;

	M[i++]=a2+b2-c2-d2;
	M[i++]=2.f*(bc+ad);
	M[i++]=2.f*(bd-ac);
	M[i++]=0.f;
	M[i++]=2.f*(bc-ad);
	M[i++]=a2-b2+c2-d2;
	M[i++]=2.f*(cd+ab);
	M[i++]=0.f;
	M[i++]=2.f*(bd+ac);
	M[i++]=2.f*(cd-ab);
	M[i++]=a2-b2-c2+d2;
	M[i++]=0.f;
	M[i++]=translation.x;
	M[i++]=translation.y;
	M[i++]=translation.z;
	M[i++]=1.f;

	return result;
	}


Matrix4 Matrix4::getTransposed() const
{
	Matrix4 result;
	for (int i=0;i<AMT_MATRIX_4_ROWS_OR_COLUMNS;i++)
		{
		for (int j=0;j<AMT_MATRIX_4_ROWS_OR_COLUMNS;j++)
			{
			result.setCell(i,j,getCell(j,i));
			}
		}
	return result;
	}

std::string Matrix4::asString()
	{
	float* M=getAsFloatArray();
	std::stringstream ss;
	ss.str("");
	std::string rows;
	int i=0;
	ss.precision(4);
	ss<<"["<<M[i+0]<<","<<M[i+1]<<","<<M[i+2]<<","<<M[i+3]<<"]"<<std::endl; i+=4;
	ss<<"["<<M[i+0]<<","<<M[i+1]<<","<<M[i+2]<<","<<M[i+3]<<"]"<<std::endl; i+=4;
	ss<<"["<<M[i+0]<<","<<M[i+1]<<","<<M[i+2]<<","<<M[i+3]<<"]"<<std::endl; i+=4;
	ss<<"["<<M[i+0]<<","<<M[i+1]<<","<<M[i+2]<<","<<M[i+3]<<"]"<<std::endl; i+=4;
	return ss.str();
	}

void Matrix4::rotateRadiansAboutAxis(float radians,MatrixDimension dimA,MatrixDimension dimB)
	{
	makeIdentity();
	float cosval=cos(radians);
	float sinval=sin(radians);
	setCell(dimA,dimA,cosval);
	setCell(dimA,dimB,-sinval);
	setCell(dimB,dimA,sinval);
	setCell(dimB,dimB,cosval);
	}

Matrix4 Matrix4::makeOrthoProjection(float nearX, float farX, float nearY, float farY, float nearZ, float farZ)
	{
	Matrix4 result;
	float sx = 1.0f / (farX-nearX);
	float sy = 1.0f / (farY-nearY);
	float sz = 1.0f / (farZ-nearZ);

	result.i = Vector4(2.0f * sx, 0.0f, 0.0f, 0.0f);
	result.j = Vector4(0.0f, 2.0f * sy, 0.0f, 0.0f);
	result.k = Vector4(0.0f, 0.0f, sz, 0.0f);
	result.t = Vector4(-(farX + nearX) * sx, -(farY + nearY) * sy, -(nearZ) * sz, 1.0f);
	
	return result;
	}
