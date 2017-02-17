#pragma once

typedef unsigned int uint;

template<typename T>
inline void memZero(T* ptr)
	{
	memset(ptr, 0, sizeof(T));
	}


enum BufferUsage: uint
	{
	BUFFERUSAGE_GPU,     // GPU Read/Write, CPU Full Writes        (RenderTargets?)
	BUFFERUSAGE_STATIC,  // GPU Read, Written only at creation     (TEXTURES, STATIC MESHES)
	BUFFERUSAGE_DYNAMIC, // GPU Read, CPU Write (Partial or Full)  (DYNAMIC MESHES/DYNAMIC TEXTURES)
	BUFFERUSAGE_STAGING, // GPU Read/Write, CPU Read/Write         Slowest - image composition/manipulation
	};

#define SAFE_DELETE(ptr)   if (nullptr != (ptr)) { delete ptr; ptr = nullptr; }