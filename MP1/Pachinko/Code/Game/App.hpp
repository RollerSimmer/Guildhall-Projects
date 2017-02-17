#pragma once
#include "Game/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"

const int FRAME_MILLISECONDS=0;
const float FRAME_SECONDS=(float)FRAME_MILLISECONDS/1000.f;

class App
{
public:
	App();
	~App();
	void Render();
	void Update(float deltaSeconds);
	void RunFrame();
	void registerKeyDown(unsigned char ikey);
	void registerKeyUp(unsigned char ikey);
	bool isKeyDown(unsigned char ikey);
	void setIsQuitting(bool quitstate);
	bool isQuitting();
	void toggleSlowmo();
	void drawWorldBounds();
	void togglePause();
	void beginFrame();
	void endFrame();

private:
	Game* m_game;
	float m_frameSeconds;
	bool m_isQuitting;
	bool m_pauseState;
	bool m_slowmoState;
	float m_pauseFactor;		
	};