#pragma once
#include "Engine/Renderer/RHI/RHIOutputMode.hpp"
#include "Engine/Gui/Window.hpp"

class RHIDeviceContext;
class RHIOutput;

class RHIInstance
	{
	public:
		// Constructor is private:  Singleton Class
		~RHIInstance();

		bool createOutput(
										WindowMessageHandlerCallback winproc,
										RHIDeviceContext*& out_context,
										RHIOutput*& out_output,
										const std::string& windowTitle,
										int left,
									    int top,
										int width,
										int height,
										/*OPTIONAL*/ RHIOutputMode const initial_mode = RENDEROUTPUT_WINDOWED);

		static RHIInstance* getInstance();
	private:
		RHIInstance();

	};