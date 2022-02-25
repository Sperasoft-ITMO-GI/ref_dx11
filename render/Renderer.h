#pragma once

#include <memory>
#include <tuple>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Window.h>

// for now it will be a singletone with base dx11 structured reailized

//	cs_5_0	DirectCompute 5.0 (compute shader)
//	ds_5_0	Domain shader
//	gs_5_0	Geometry shader
//	hs_5_0	Hull shader
//	ps_5_0	Pixel shader
//	vs_5_0	Vertex shader

#define CS_SHADER_ENTRY 0b0000'0001
#define DS_SHADER_ENTRY 0b0000'0010
#define GS_SHADER_ENTRY 0b0000'0100
#define HS_SHADER_ENTRY 0b0000'1000
#define PS_SHADER_ENTRY 0b0001'0000
#define VS_SHADER_ENTRY 0b0010'0000

struct ShaderOptions
{
	D3D_SHADER_MACRO* mac;
	int entrys;
};

class Renderer {
public:
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	~Renderer();

	static Renderer* GetInstance();

	bool Initialize(const HINSTANCE instance, const WNDPROC wndproc);

	void BeginEvent(wchar_t* name);
	void EndEvent();
	void SetMarker(wchar_t* name);

public:
	void SetWindowMode(const int posX, const int posY, const int w, const int h, const bool fullscreen);
	void SetPerspectiveMatrix(const float fov, const float aspect_ratio, const float z_near, const float z_far);
	void SetModelViewMatrix(const DirectX::XMMATRIX& model_view_mx);

	void AddTexturetoSRV(int width, int height, int bits, unsigned char* data, int texNum, bool mipmap);
	void UpdateTextureInSRV(int width, int height, int xOffset, int yOffset, int bits, unsigned char* data, int texNum);
	void CreateSkyBoxSRV();

	void AddCustomTextureToSrv(char* path, int texNum, bool mipmap);

	void IterativeUpdateSkyBoxSrv(int width, int height, int bits, unsigned char* data);

	void DeleteTextureFromSRV(int texNum);

	void Test(char* name, int width, int height, int bits, unsigned char* data, int type);

	void InitMatrix(int widht, int height);

	DirectX::XMMATRIX GetOrthogonal();
	DirectX::XMMATRIX GetPerspective();
	DirectX::XMMATRIX GetModelView();

	void Clear() {
		context->ClearRenderTargetView(render_target_view, DirectX::Colors::Black);
		context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void Swap() {
		swap_chain->Present(1, 0);
	}

	void SetDepthBuffer() {
		context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
	}

	void UnSetDepthBuffer() {
		context->OMSetRenderTargets(1, &render_target_view, nullptr);
	}

	void Bind(int texture_index, int textureSlot);
	void BindSkyBox();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	IDXGISwapChain* GetSwapChain();
	ID3D11RenderTargetView* GetRenderTargetView();
	std::tuple<float, float> GetWindowParameters();
	UINT GetMSAAQuality() {
		return msaa_quality;
	}
	bool IsMsaaEnable() {
		return is_4xmsaa_enable;
	}
	ID3D11ShaderResourceView* GetSkyBoxSRV() {
		return sky_box_view;
	}

	unsigned char** GetSkyBoxData() {
		return sky_data;
	}

	ID3D11DepthStencilView* GetDepthStencilView() {
		return depth_stencil_view;
	}

private:
	Renderer();

private:
	std::unique_ptr<Window> window;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swap_chain;
	ID3D11DepthStencilView* depth_stencil_view;
	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11RenderTargetView* render_target_view;

	D3D11_VIEWPORT viewport;

	D3D_DRIVER_TYPE driver_type;
	UINT msaa_quality;
	bool is_4xmsaa_enable;
	bool is_initialized;

	int skyBoxIterator = 0;
	int sky_width = 256;
	int sky_bits = 32;
	unsigned char** sky_data;

	ID3DUserDefinedAnnotation* perf;
	
	ID3D11SamplerState* sampler;
	ID3D11ShaderResourceView* texture_array_srv[1600];

	ID3D11ShaderResourceView* sky_box_view;

	DirectX::XMMATRIX orthogonal;
	DirectX::XMMATRIX perspective;
	DirectX::XMMATRIX model_view;

private:
	static Renderer* renderer;
};