#pragma once

#include <string>

#include "WndDxIncludes.h"
#include "Shader.h"

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

#if defined(DEBUG) | defined(_DEBUG)
#ifndef DXCHECK
#define DXCHECK(x)                                                   \
	{                                                           \
		HRESULT hr = (x);                                       \
		if(FAILED(hr))                                          \
		{                                                       \
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                       \
	}
#endif
#endif

std::wstring ToWide(const std::string& narrow);

Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const wchar_t* filename, const D3D_SHADER_MACRO* defines,
                                               const char* entrypoint, const char* target);

DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync);