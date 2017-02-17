#pragma once


const float SCREEN_SHAKE_AMOUNT=10.f;
const float SCREEN_SHAKE_TIME_SPAN = 0.5f;

class Game
	{
	private:
		int m_timescale;
		bool m_isPaused;
		int m_gameState;
		float m_endFrameProgression;
		bool m_isQuittingGame;
		float m_currentShakeAmount;
		float m_currentShakeTimeLeft;
	public:
		Game();
		void init();

		void update(float deltaSeconds);
			void updateScreenShakeTimer(float deltaSeconds);

		void render() const;
			void doScreenShake() const;

		void startScreenShake();
		void toggleDebugGraphics();
		void togglePause();
		bool isQuitting();
	};
