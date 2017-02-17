#pragma once
#include "Engine\Renderer\Camera3D.hpp"
#include "Engine\Renderer\SpriteSheet.hpp"
#include "Game/World.hpp"

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

		void initSettings();
		void saveSettings();

		void update(float deltaSeconds);
			void updateScreenShakeTimer(float deltaSeconds);
			void updateInput(float deltaSeconds);
				void updateRotationInput(float deltaSeconds);
				void updatePositionInput(float deltaSeconds);
					void updatePlayerFromMouse();
					void updateCameraFromMouse();
					void updateEditingInput(float deltaSeconds);
					void updateBlockSelectionInput(float deltaSeconds);
					void updateCameraModeInput();
					void bringPlayerEyeToNoClipCam();
					void updateMovementModeInput();
			void updateFaceSelector();
			void updateCamera();
				void fixThirdPersonCamModes();

		void render() const;
			void doScreenShake() const;
			void goIntoSceneRenderMode() const;
			void goIntoTextRenderMode() const;
			void displayGameDebugInfo() const;
				std::string makeDebugLines() const;
			void drawCrosshair() const;
			void drawDebugDots() const;
			void drawSelectedFaceBox() const;
			void drawViewFrustum() const;
			void displayHUD() const;
				void displayHUDBackground() const;
				void displayHUDBlocks() const;
					void drawSingleHUDBlock(int blockIndex) const;
					void drawSelectedHUDBlockOutline() const;
				void displayHUDShortcutKeys() const;
					void drawSingleBlockSelectorShortcut(int blockIndex) const;

		void startScreenShake();
		void toggleDebugGraphics();
		void togglePause();
		bool isQuitting();

		void deactivateAllChunks();

		void addDebugDotAt(const Vector3& pos);
	private:
	public:
	private:
		void doUnitTests();

	private:
		int m_timescale;
		bool m_isPaused;
		float m_endFrameProgression;
		bool m_isQuittingGame;
		float m_currentShakeAmount;
		float m_currentShakeTimeLeft;

		RaycastResult m_currentRaycast;

		World* m_world;

		Camera3D m_cam;
		SpriteSheet* m_tileAtlas;

		std::vector<Vector3> m_debugDotPoints;

		BLOCK::Type m_selectedPlacementBlock;

		SoundID m_menuSelectSound;
		AudioChannelHandle m_menuSelectAudioChannel;
		void updateVisualInput(float deltaSeconds);
	};
