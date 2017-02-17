#pragma once
#include "Game/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"

const int FRAME_MILLISECONDS=16;

class App
{
public:
	App();
	~App();
	void Render() const;
	void Update(float deltaSeconds);
	void RunFrame();
	void registerKeyDown(unsigned char ikey);
	void registerKeyUp(unsigned char ikey);
	bool isKeyDown(unsigned char ikey);
	void setIsQuitting(bool quitstate);
	bool isQuitting();
	void createAsteroidInGame();
	void destroyFurthestAsteroidInGame();
	void drawWorldBounds() const;
	void spawnTheShip();
	void togglePause();
	void beginFrame();
	void endFrame();

private:
	Game* m_game;
	bool m_isQuitting;
	bool m_pauseState;
	bool m_slowmoState;
	float m_pauseFactor;		
	};