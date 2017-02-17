#pragma once

enum RHIOutputMode : unsigned int
	{
	RENDEROUTPUT_WINDOWED,
	RENDEROUTPUT_BORDERLESS,
	RENDEROUTPUT_FULLSCREEN_WINDOW,     // Requires Window Optional and ability to resize SwapChain
	RENDEROUTPUT_FULLSCREEN_DEDICATED,  // Dedicated Fullscreen Mode.  
	};
