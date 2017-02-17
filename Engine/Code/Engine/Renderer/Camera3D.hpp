#pragma once
#include "Engine\Math\Vector3.hpp"
#include <string>
#include <vector>

namespace CAMERA
	{
	enum ModeType
		{
		FIRST_PERSON=0,
		FROM_BEHIND,
		FIXED_ANGLE,
		NO_CLIP,
		AMT_MODES
		};
	}

class Camera3D
	{
	public:
		Camera3D();

		std::vector<Vector3> getRightForwardUp();
		Vector3 getForwardXY() const;
		Vector3 getLeftXY() const;
		Vector3 getForward() const;
		std::string getModeName() const;	
		std::string asString() const;
		void selectNextMode();
		std::vector<Vector3> getViewFrustumPlaneNormals();

	public:
		static bool s_haveStaticsBeenInitialized;
		static std::string s_modeNames[CAMERA::AMT_MODES];

	public:
		Vector3 m_pos;
		float m_rollDegreesAboutX;
		float m_pitchDegreesAboutY;
		float m_yawDegreesAboutZ;
		CAMERA::ModeType m_mode;
		float m_fovVerticalDegrees;
		float m_aspectRatio;
		float m_nearDistance;
		float m_farDistance;

		Vector3 m_viewFrustumPoints[5];

	};

