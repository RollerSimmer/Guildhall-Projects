#pragma once
#include "Game/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"

const int FRAME_MILLISECONDS=16;
const double MAX_FRAME_DURATION_SECONDS=0.1f;

class App
{
public:
	App();
	~App();
	void render() const;
	
	void update(float deltaSeconds);
	void runFrame();
		void beginFrame();
		void endFrame();

	void registerMouseMove(unsigned short mouseflags,const IntVector2& mousepos);
	void initMousePosition(unsigned short mouseflags,const IntVector2& mousepos);

	void registerMouseButtonDown(unsigned short mouseflags,const IntVector2& mousepos);
	void registerMouseButtonUp(unsigned short mouseflags,const IntVector2& mousepos);
	void registerMouseDoubleClick(unsigned short mouseflags,const IntVector2& mousepos);
	void stabilizeMousePos();
	void resetMouseButtons();

	void onGainedFocus();
	void onLostFocus();

	void registerKeyDown(unsigned char ikey);
	void registerKeyUp(unsigned char ikey);
	bool isKeyDown(unsigned char ikey);
	
	void setIsQuitting(bool quitstate);
	bool isQuitting();
	
	void drawWorldBounds() const;
	
	void togglePause();

private:
	Game* m_game;
	bool m_isQuitting;
	bool m_pauseState;
	bool m_slowmoState;
	float m_pauseFactor;		
	};