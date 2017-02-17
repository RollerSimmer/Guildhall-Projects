#pragma once
#include "Game/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"

const int FRAME_MILLISECONDS=16;

// Master To-do list for Incursion
//TODO: load maps from map files
//TODO: create SpriteAnim class in Engine/Renderer directory and use it somehow
//TODO: add ability to progress through levels - level progression could be based on either number of kills or reaching a target tile
//TODO: add game over screens for both defeat and victory perhaps?
//TODO: add some more "juice" to the game

class App
{
public:
	App();
	~App();

	Game* getGame() const { return m_game; }

	void Render() const;
	void Update(float deltaSeconds);
	void RunFrame();
	void registerKeyDown(unsigned char ikey);
	void registerKeyUp(unsigned char ikey);
	bool isKeyDown(unsigned char ikey);
	void setIsQuitting(bool quitstate);
	bool isQuitting();
	void drawWorldBounds() const;
	void togglePause();
	void beginFrame();
	void endFrame();

private:
	Game* m_game;
	bool m_isQuitting;
	bool m_pauseState;
	bool m_slowmoState;
	float m_pauseFactor;		
	void toggleDebugGraphics();
	};