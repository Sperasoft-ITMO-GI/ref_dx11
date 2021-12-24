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
#include <vector>
#include <utility>
#include <stdexcept>

#include <Utils.h>

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

//DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync);

// TODO: NEED TO RENAME!!!
struct Vertex_PosTexCol 
{
	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT4 col;
};

struct Vertex_PosTexCol_Info
{
	struct Pos {
		float x, y, width, height;
	} pos;

	struct TexCoord {
		float xl, yt, xr, yb;
	} tex;

	struct Col {
		float x, y, z, w;
	} col;
};

struct Vertex {
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT4 color;
};

struct ConstantBufferStr {
	DirectX::XMMATRIX transform;
};

class InitDX11
{
public:
		
	bool InitializeDX11(HINSTANCE hinstance_, WNDPROC wndProc_);
	void SetMode(int width_, int height_, bool fullscreen_);
	void ClearScene();
	void SwapBuffers();

	bool CompileAllShaders();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadTestBMP(char* fileName);

	void DrawColored2D(std::array<std::pair<float, float>, 4> vertexes, std::array<float, 4> color);

	~InitDX11();

	void AddTexturetoSRV(int width, int height, int bits, unsigned char* data, int texNum, bool dynamic);
	void UpdateTextureInSRV(int width, int height, int bits, unsigned char* data, int texNum);

	void DummyTest(char* name, int width, int height, int bits, unsigned char* data, int type);
	void DummyDrawingPicture(Vertex_PosTexCol_Info* vertex, int texNum);

private:

	bool InitializeWindow(HINSTANCE hinstance_, WNDPROC wndProc_);
	// TODO: need to rename it
	void CreateConstantVar();
	bool CompileQuadShaders();

	bool					enable4xMsaa;
	UINT					msaaQuality;

	ID3D11Device*			d3dDevice;
	ID3D11DeviceContext*	d3dImmediateContext;
	IDXGISwapChain*			swapChain;
	ID3D11Texture2D*		depthStencilBuffer;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11BlendState*		blendState;
	ID3D11RasterizerState*	d3dRasterizerState;
	D3D11_VIEWPORT			screenViewport;
	D3D_DRIVER_TYPE			d3dDriverType;

	HWND					mainWindow;

	HINSTANCE				hInstance;
	WNDPROC					wndProc;

	DirectX::XMMATRIX		orthogonal;

	// TODO: temporable variables
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_PosColTex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_PosColTex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_PosColTex_layout;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texArraySRV[1600];
	

	Microsoft::WRL::ComPtr<ID3D11SamplerState> d3dSamplerState;

	int mainWindowWidth;
	int mainWindowHeight;

	bool fullscreen;

	char* windowName = "Quake 2 DX11";
	char* windowClassName = "Quake 2";
};