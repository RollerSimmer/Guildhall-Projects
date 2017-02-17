#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "GL/GL.h"
#include "GL/GLU.h"

#include "ThirdParty/OpenGL/glext.h"

extern PFNGLGENBUFFERSPROC		glGenBuffers;
extern PFNGLBINDBUFFERPROC		glBindBuffer;
extern PFNGLDELETEBUFFERSPROC	glDeleteBuffers;
extern PFNGLBUFFERDATAPROC		glBufferData;
extern PFNGLGENERATEMIPMAPPROC	glGenerateMipmap;