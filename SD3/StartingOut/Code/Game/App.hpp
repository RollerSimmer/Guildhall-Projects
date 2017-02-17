#pragma once
#include "Game/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Gui/Window.hpp"
#include "Engine/Renderer/SimpleRendererDx.hpp"

const int FRAME_MILLISECONDS=16;

class RHIDevice;
class RHIDeviceContext;
class RHIOutput;
class Sampler;

class App
{
public:
	App();
	~App();
	void start();
	void end();
	void render() const;
	void Update(float deltaSeconds);
	
	void run();
	void runMessagePump();
	void runFrame();
	
	void registerKeyDown(unsigned char ikey);
	void registerKeyUp(unsigned char ikey);
	bool isKeyDown(unsigned char ikey);
	void setIsQuitting(bool quitstate);
	bool isQuitting();
	void drawWorldBounds() const;
	void togglePause();
	void beginFrame();
	void endFrame();

	void initRendering();
	void setMsgProc(WindowMessageHandlerCallback msgProc) {m_msgProc=msgProc;}

private:
	SimpleRendererDx* m_renderer;

	Game* m_game;
	bool m_isQuitting;
	bool m_pauseState;
	bool m_slowmoState;
	float m_pauseFactor;	
	RHITexture2D* m_texSample;
	Sampler* m_pointSampler;
	
	RHIDevice* m_rhiDevice;
	RHIDeviceContext* m_rhiContext;
	RHIOutput* m_rhiOutput;

	ShaderProgram* m_myShader;
	VertexBuffer* m_triVbo;

	WindowMessageHandlerCallback m_msgProc;
	};