#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Math/MathUtil.hpp"
#include <sstream>
#include <vector>

bool Camera3D::s_haveStaticsBeenInitialized=false;

std::string Camera3D::s_modeNames[CAMERA::AMT_MODES];

Camera3D::Camera3D()
	: m_pos(Vector3::ZERO)
	, m_rollDegreesAboutX(0.f)
	, m_pitchDegreesAboutY(0.f)
	, m_yawDegreesAboutZ(0.f)
	, m_mode(CAMERA::FIRST_PERSON)
	, m_fovVerticalDegrees(0.f)
	, m_aspectRatio(0.f)
	, m_nearDistance(0.f)
	, m_farDistance(0.f)
	{
	if (!s_haveStaticsBeenInitialized)
		{
		s_modeNames[CAMERA::FIRST_PERSON]=std::string("First-Person");
		s_modeNames[CAMERA::FROM_BEHIND]=std::string("From-Behind");
		s_modeNames[CAMERA::FIXED_ANGLE]=std::string("Fixed-Angle");
		s_modeNames[CAMERA::NO_CLIP]=std::string("No-Clip");
		s_haveStaticsBeenInitialized=true;
		}
	}

std::vector<Vector3> Camera3D::getRightForwardUp()
	{
	std::vector<Vector3> result;
	Vector3 fwd;
	Vector3 right;
	Vector3 up,worldUp;
	fwd=getForward();
	worldUp=Vector3(0.f,0.f,1.f);
	right=fwd.crossProduct(worldUp);
	right.normalize();
	up=right.crossProduct(fwd);
	up.normalize();
	result.push_back(right);
	result.push_back(fwd);
	result.push_back(up);
	return result;
	}

Vector3 Camera3D::getForwardXY() const
	{
	return Vector3(cosDegrees(m_yawDegreesAboutZ),sinDegrees(m_yawDegreesAboutZ),0.f);
	}

Vector3 Camera3D::getLeftXY() const
	{
	Vector3 forward=getForwardXY();
	return Vector3(-forward.y,forward.x,0.f);
	}

Vector3 Camera3D::getForward() const
	{
	Vector3 result(1.f,0.f,0.f);
	result.rotateDegrees(m_pitchDegreesAboutY,Y_AXIS_3D);
	result.rotateDegrees(m_yawDegreesAboutZ,Z_AXIS_3D);
	result.normalize();
	return result;
	}

std::string Camera3D::getModeName() const
	{
	if(m_mode>=CAMERA::FIRST_PERSON && m_mode<CAMERA::AMT_MODES)
		return s_modeNames[m_mode];
	else
		return std::string("Bad-Camera-Mode");
	}

std::string Camera3D::asString() const
	{
	std::stringstream ss;
	ss.str("");
	ss<<"{pos = "<<m_pos.asString()<<","/*<<std::endl*/;
	ss<<"rollX = "<<m_rollDegreesAboutX<<","/*<<std::endl*/;
	ss<<"pitchY = "<<m_pitchDegreesAboutY<<","/*<<std::endl*/;
	ss<<"yawZ = "<<m_yawDegreesAboutZ<<"}"/*<<std::endl*/;
	return ss.str();
	}

void Camera3D::selectNextMode()
	{
	int modeAsInt=(int)m_mode;
	if(modeAsInt<=0)
		modeAsInt=0;
	modeAsInt++;
	if(modeAsInt>=CAMERA::AMT_MODES)
		modeAsInt=0;
	m_mode=(CAMERA::ModeType)modeAsInt;
	}

std::vector<Vector3> Camera3D::getViewFrustumPlaneNormals()
	{
	float halfVerticalDegrees=m_fovVerticalDegrees*0.5f;

	std::vector<Vector3> result;

	Vector3 right,up,fwd;
	std::vector<Vector3> rfu=getRightForwardUp();
	if (true)
		{
		int i=0;
		right=rfu.at(i++);
		fwd=rfu.at(i++);
		up=rfu.at(i++);
		}

	float k=sinDegrees(-halfVerticalDegrees);
	float j=cosDegrees(-halfVerticalDegrees);
	float i=k*m_aspectRatio;

	Vector3 I=i*right;
	Vector3 J=j*fwd;
	Vector3 K=k*up;

	Vector3 upLeft=-I+J+K;
	Vector3 upRight=I+J+K;
	Vector3 downLeft=-I+J-K;
	Vector3 downRight=I+J-K;

	upLeft.normalize();
	upRight.normalize();
	downLeft.normalize();
	downRight.normalize();

	Vector3 rightSideNormal=upRight.crossProduct(downRight);
	Vector3 leftSideNormal=downLeft.crossProduct(upLeft);
	Vector3 topNormal=upLeft.crossProduct(upRight);
	Vector3 bottomNormal=downRight.crossProduct(downLeft);

	rightSideNormal.normalize();
	leftSideNormal.normalize();
	topNormal.normalize();
	bottomNormal.normalize();

	result.push_back(rightSideNormal);
	result.push_back(leftSideNormal);
	result.push_back(topNormal);
	result.push_back(bottomNormal);

	float viewFrustumEdgeLength=500.f;

	if (true)
		{
		int ii=0;
		m_viewFrustumPoints[ii++]=(m_pos);
		m_viewFrustumPoints[ii++]=(m_pos+upRight*viewFrustumEdgeLength);
		m_viewFrustumPoints[ii++]=(m_pos+upLeft*viewFrustumEdgeLength);
		m_viewFrustumPoints[ii++]=(m_pos+downLeft*viewFrustumEdgeLength);
		m_viewFrustumPoints[ii++]=(m_pos+downRight*viewFrustumEdgeLength);
		}

	return result;	
	}

