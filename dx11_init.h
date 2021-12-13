#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <comdef.h>

#include <stdio.h>

#include "dxerr.h"

#include <array>
#include <utility>
#include <stdexcept>

//---------------------------------------------------------------------------------------
// Simple d3d error checker for book demos.
//---------------------------------------------------------------------------------------

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }

DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync);

struct Vertex {
	//float x, y;
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT4 color;
};

struct ConstantBuffer {
	DirectX::XMMATRIX transform;
};

class InitDX11
{
public:
		
	bool InitializeDX11(HINSTANCE hinstance_, WNDPROC wndProc_);
	void SetMode(int width_, int height_, bool fullscreen_);
	void DrawScene();

	void DrawColored2D(std::array<std::pair<float, float>, 4> vertexes, std::array<float, 4> color);

	~InitDX11();

private:

	bool InitializeWindow(HINSTANCE hinstance_, WNDPROC wndProc_);

	bool					enable4xMsaa;
	UINT					msaaQuality;

	ID3D11Device*			d3dDevice;
	ID3D11DeviceContext*	d3dImmediateContext;
	IDXGISwapChain*			swapChain;
	ID3D11Texture2D*		depthStencilBuffer;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT			screenViewport;
	D3D_DRIVER_TYPE			d3dDriverType;

	HWND					mainWindow;

	HINSTANCE				hInstance;
	WNDPROC					wndProc;

	int width;
	int height;

	bool fullscreen;

	char* windowName = "Quake 2 DX11";
	char* windowClassName = "Quake 2";
};