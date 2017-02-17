#pragma once
#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/SceneGraph.hpp"
#include "Game/CelestialNode.hpp"


const int CURRENT_GAME_VERSION = 0;

const float SCREEN_SHAKE_AMOUNT=10.f;
const float SCREEN_SHAKE_TIME_SPAN = 0.5f;

const float ISOMETRIC_BLOCK_WIDTH_PADDING_FACTOR = 1.2f;
const float ISOMETRIC_BLOCK_WIDTH = 50.f;

struct SettingsFileData
	{
	unsigned int m_version;
	unsigned int m_seed;
	Camera3D m_camera;
	};

class Game
	{
	public:
		Game();
		void init();
		~Game();

		Camera3D& getCamera() { return m_cam; }

		void update(float deltaSeconds);
			void updateScreenShakeTimer(float deltaSeconds);
			void updateInput(float deltaSeconds);
				void updateRotationInput(float deltaSeconds);
				void updatePositionInput(float deltaSeconds);
				void updateCameraFromMouse();
			void updateCamera();

		void render();

			void doScreenShake();
			void goIntoSceneRenderMode();
			void goIntoTextRenderMode();
			void displayGameDebugInfo();
				std::string makeDebugLines();
			void displayHUD();
			void drawDebugDots();

		void startScreenShake();
		void toggleDebugGraphics();
		void togglePause();
		bool isQuitting();

		void addDebugDotAt(const Vector3& pos);

		void initSolarSystem();

		void jumpToNextPlanet();
		void cycleFocusPlanet();
		void lockCameraPositionToPlanet();

		bool lookAtFocus();
		void updateCurrentFocusBodyMessage();

	private:
	public:
	private:
		void doUnitTests();

	private:
		int m_timescale;
		bool m_isPaused;
		bool m_isQuittingGame;
		float m_currentShakeAmount;
		float m_currentShakeTimeLeft;

		int m_targetIndex;
		CelestialNode* m_targetNode;
		int m_focusIndex;
		CelestialNode* m_focusNode;

		SceneGraph m_solarSystem;

		std::vector<CelestialNode*> m_nodeList;

		Camera3D m_cam;

		std::vector<Vector3> m_debugDotPoints;
	};
