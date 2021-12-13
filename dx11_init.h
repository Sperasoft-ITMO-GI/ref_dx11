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
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT4 color;
};

struct ConstantBuffer {
	DirectX::XMMATRIX transform;
};

// TODO: need to rename it
struct TexturedQuad_Info
{
	D3D_PRIMITIVE_TOPOLOGY topology;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	unsigned short indices;
	UINT vertex_stride;
	UINT vertex_offset;
};

// TODO: NEED TO RENAME!!!
struct TextureQuad_Instance
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	DirectX::XMMATRIX transform;
};

class InitDX11
{
public:
		
	bool InitializeDX11(HINSTANCE hinstance_, WNDPROC wndProc_);
	void SetMode(int width_, int height_, bool fullscreen_);
	void ClearScene();
	void SwapBuffers();

	void AddPictureToArray(int x, int y, int width, int height);

	void DrawPic();

	bool CompileAllShaders();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadTestBMP(char* fileName);

	void DrawColored2D(std::array<std::pair<float, float>, 4> vertexes, std::array<float, 4> color);

	~InitDX11();

private:

	bool InitializeWindow(HINSTANCE hinstance_, WNDPROC wndProc_);
	// TODO: need to rename it
	void CreateConstantVar();
	void BuildQuadStaff();
	bool CompileQuadShaders();

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

	DirectX::XMMATRIX		orthogonal;

	TexturedQuad_Info		textureQuad;
	std::vector<TextureQuad_Instance> quads;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> d3dSamplerState;

	int width;
	int height;

	bool fullscreen;

	char* windowName = "Quake 2 DX11";
	char* windowClassName = "Quake 2";
};