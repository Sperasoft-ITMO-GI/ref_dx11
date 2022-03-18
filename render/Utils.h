#pragma once

#include <string>
#include <vector>

#include <WndDxIncludes.h>
#include <Shader.h>
#include <Topology.h>
#include <VertexShader.h>
#include <PixelShader.h>

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

#else

#define DXCHECK(x) x

#endif

#define DEBUG_MARKERS

#ifdef DEBUG_MARKERS

#define BEGIN_EVENT(X) renderer->BeginEvent(X);
#define END_EVENT() renderer->EndEvent();
#define SET_MARKER(X) renderer->SetMarker(X);

#else

#define BEGIN_EVENT(X)
#define END_EVENT()
#define SET_MARKER(X) 

#endif

std::wstring ToWide(const std::string& narrow);

ID3DBlob* CompileShader(const wchar_t* filename, const D3D_SHADER_MACRO* defines,
                                               const char* entrypoint, const char* target);

DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync);

void SmartTriangulation(std::vector<uint16_t>* ind, int num, int offset);

void SmartTriangulationClockwise(std::vector<uint16_t>* ind, int num, int offset);

void TriangulationTriangleStripToList(std::vector<uint16_t>* ind, int num, int offset);

void TriangulationTriangleStripToListClockwise(std::vector<uint16_t>* ind, int num, int offset);
