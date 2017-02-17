#pragma once
#include "Engine/Math/Snowman.hpp"
#include "Game/BlockInfo.hpp"
#include "Engine/Audio/Audio.hpp"
#include <vector>

/*

on player).
d. (2 points) The player primitive is sized exactly thus:
i. 1.86m (1.86 blocks / world units) tall, top to bottom;
ii. 0.60m wide (e.g. 0.30m radius for cylinder/capsule, 0.60m x-width and y-width for AABB3).

*/

const float PLAYER_SNOWMAN_RADIUS = 0.6f*0.5f;
const float PLAYER_SNOWMAN_HALF_HEIGHT = 1.86f*0.5f;
const float PLAYER_MAX_AIR_JUMP_COUNT = 1;
const float PLAYER_WALL_JUMP_WINDOW_SECONDS = 0.1f;
const float PLAYER_MAX_WALK_STRIDE_LENGTH = 2.f;
const float PLAYER_MIN_WALK_STRIDE_LENGTH = 0.5f;

namespace MOVE
	{
	enum Mode
		{
		WALK=0,
		FLY,
		AMT_MODES
		};
	}


class Player
	{
	public:
		Player(const Vector3& startingPosition);
		void init();

	public:
		void update(float deltaSeconds);
			void accelerate(float deltaSeconds);
				Vector3 calcAcceleration();
			void move(float deltaSeconds);
			void jumpAutomatically(float deltaSeconds);
			void updateDebugMessages();
			void updateWallJumpInfo(float deltaSeconds);
		void detectAndCorrectCollisions(float deltaSeconds);

		void render() const;

		void teleportTo(const Vector3& pos);
		Vector3 getPosition();
		Vector3 calcEyePosition();
		Vector3 calcFromBehindPosition();
		void setPosition(const Vector3& pos);
		Vector3 calcHeadForwardVector();
		void cycleMovementMode();

		void startJump();
		bool canJump();
		void debugDisplayAirAndWallJumpInfo();
		void playJumpSound();
		void neutralizeVelocityVerticalityBeforeJumping();

		bool isWalking();
		void walkStep(float deltaSeconds);
			bool shouldPlayWalkSound();
			void playWalkSound();
				std::vector<int> getGroundTypesUnderMe();
				void resetWalkStepCountdown();
		bool didJustLand();

		void playBlockRemovalSound(unsigned char blockType);
		void playBlockPlacementSound(unsigned char blockType);

	public: 
		Snowman m_playerSnowman;
		Vector3 m_prevPos;
		Vector3 m_accel;
		Vector3 m_vel;
		Vector3 m_moveAccel;
		Vector3 m_jumpVel;
		int m_airJumpCount;
		Vector3 m_flyAccel;
		float m_yawDegrees;
		float m_headPitchDegrees;

		float m_walkDistanceLeft;
		AudioChannelHandle m_walkStepAudioHandle;

		bool m_isGrounded;
		bool m_wasGroundedLastFrame;
		float m_wallJumpTimeLeftSeconds;

		int m_moveMode;

		float m_autoJumpCooldownSeconds;

		SoundID m_jumpSound;
		AudioChannelHandle m_jumpAudioHandle;
		AudioChannelHandle m_placementAudioHandle;
		AudioChannelHandle m_removalAudioHandle;

	public:

	private:
		void addVelocityInfoToDebugPrinter();

		std::vector<BlockInfo> getMyGroundNeighbors();
	};

