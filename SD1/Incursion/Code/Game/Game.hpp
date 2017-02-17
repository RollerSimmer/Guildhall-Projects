#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Game/Map.hpp"

const float SCREEN_SHAKE_AMOUNT=10.f;
const float SCREEN_SHAKE_TIME_SPAN = 0.5f;

const int POINTS_FOR_ENEMY_KILL = 100;
const int POINTS_DEDUCTED_FOR_PLAYER_KILL = 100;
const int PLAYER_DAMAGE_NEGATIVE_SCORE_FACTOR = -10;
const int ENEMY_DAMAGE_POSITIVE_SCORE_FACTOR = 1;


enum GameState
	{
	GAME_STATE_ATTRACT,
	GAME_STATE_GAME_OVER,
	GAME_STATE_PAUSED,
	GAME_STATE_PLAYING,
	GAME_STATE_RESTART,
	GAME_STATE_CONTINUE,
	GAME_STATE_VICTORY,
	NEXT_GAME_STATE_KEEP_CURRENT,
	AMT_GAME_STATES
	};

const float GAME_OVER_SCREEN_COUNTDOWN_DURATION_SECONDS = 10.f;

class Game
	{
	private:
		int m_timescale;
		bool m_isPaused;
		float m_endFrameProgression;
		bool m_isQuittingGame;
		float m_currentShakeAmount;
		float m_currentShakeTimeLeft;
		Map* m_theMap;
		int m_score;
		GameState m_state;
		GameState m_nextState;
		float m_nextStateCountdownSeconds;		
		float m_transitionDurationSeconds;
		static Rgba s_defaultBgColor;
		Texture* m_titleBackgroundImage;
	public:
		Game();
		~Game();
		void init();

		//accessors
		GameState getGameState() { return m_state; }

		//methods

		//update
		void update(float deltaSeconds);
			void updateScreenShakeTimer(float deltaSeconds);
			void updateMap(float deltaSeconds);
			void updateGameState(float deltaSeconds);
			void updateGameOverMode(float deltaSeconds);
			void updatePausedMode(float deltaSeconds);
			void updateAttractMode(float deltaSeconds);
			void updatePlayMode(float deltaSeconds);
				void updateGameplay(float deltaSeconds);
			void updateVictoryMode(float deltaSeconds);
		float calcNextStateWaitFraction() const;

		//render
		void render() const;
			void doScreenShake() const;
			void renderPauseMode() const;
			void renderAttractMode() const;
				void renderTitleBackground() const;
			void renderPlayMode() const;
			void renderGameOverMode() const;
			void renderVictory() const;
			void drawCurrentBackgroundOrOverlay() const;

		void startScreenShake();
		void toggleDebugGraphics();

		// render alpha and color lerping
		Rgba calcStateBackgroundColor() const;
		Rgba calcAnticipatedBackgroundColor() const;
		Rgba calcCurrentBackgroundColor() const;
		static Rgba calcBackgroundColorForGameState(GameState m_state);
		float calcCurrentOverlayTextAlpha() const;
		static float calcOverlayTextAlphaForGameState(GameState m_state);

		// misc
		bool isQuitting();
		void changeScoreBy(int scoreDelta);
		bool isPaused() { return m_state==GAME_STATE_PAUSED; }
		void startStateTransition(GameState nextState,float secondsUntilTransition=0.5f);

		//input checkers
		bool wasAnyEscKeyJustPressed();
		bool wasAnyPauseKeyJustPressed();
		GameState getAnticipatedGameState();
	private:
		void drawScore() const;
		void playSoundBasedOnNewState();
	};
