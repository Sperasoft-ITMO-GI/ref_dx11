#pragma once

#include <memory>
#include <tuple>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Window.h>

#include <external/debug_console.h>
#include <shaders/shader_defines.h>

//#include <ConstantBuffer.hpp>

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

enum EffectsRTV : uint8_t {
	BACKBUFFER = 0,
	SCENE,
	SCENE_HIST, 
	SCENE_COLOR,
	MASK,
	MOTION_BLUR,
	BLOOM_1,
	BLOOM_2,
	FXAA,
	EFFECT,
	LIGHTMAP,
	POSITIONS,
	NORMALS,
	VELOSITY,
	VELOSITY_HIST,
};

enum EffectsSRV : uint8_t {
	SCENE_SRV = 0,
	SCENE_HIST_SRV,
	SCENE_COLOR_SRV,
	MASK_SRV,
	MOTION_BLUR_SRV,
	BLOOM_1_SRV,
	BLOOM_2_SRV,
	FXAA_SRV,
	EFFECT_SRV,
	LIGHTMAP_SRV,
	POSITIONS_SRV,
	NORMALS_SRV,
	VELOCITY_SRV,
	VELOCITY_HIST_SRV,
	DEPTH_SRV,
	DEPTH_HIST_SRV,
};

class Renderer {

private:
	Renderer();

private:
	std::unique_ptr<Window> window;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swap_chain;
	ID3D11DepthStencilView* depth_stencil_view[2];
	ID3D11DepthStencilState* depth_stencil_state;

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
public:
	ID3D11ShaderResourceView* texture_array_srv[1600];
	int lamp_indexes[10];
	int counter = 0;

	ID3D11ShaderResourceView* sky_box_view;

	std::unordered_map<int, std::pair<int, int>> num_texture_to_dimensions;
	//DirectX::XMMATRIX orthogonal;
	//DirectX::XMMATRIX perspective;
	//DirectX::XMMATRIX model_view;

private:
	static Renderer* renderer;

public:
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	// 0 - backbuffer
	// 1 - scene
	// 2 - ligthmap scene
	// 3 - mask
	// 4 - bloom 1
	// 5 - bloom 2
	// 6 - damage effect
	// 7 - fxaa

	int index = 0;

	static constexpr int render_textures_count = 16;
	static constexpr int render_targets_count = 15;
	static constexpr int shader_resource_views_count = 16;
	ID3D11Texture2D* render_textures[render_textures_count];
	ID3D11RenderTargetView* render_target_views[render_targets_count];
	ID3D11ShaderResourceView* shader_resource_views[shader_resource_views_count];
	ID3D11UnorderedAccessView* uav;
	
	ID3D11Texture2D* test_tex;
	ID3D11RenderTargetView* test_rtv;
	ID3D11ShaderResourceView* test_srv;

	bool is_game_started = false;

	~Renderer();

	static Renderer* GetInstance();

	bool Initialize(const HINSTANCE instance, const WNDPROC wndproc);

	void BeginEvent(wchar_t* name);
	void EndEvent();
	void SetMarker(wchar_t* name);

public:
	void SetWindowMode(const int posX, const int posY, const int w, const int h, const bool fullscreen);
	//void SetPerspectiveMatrix(const float fov, const float aspect_ratio, const float z_near, const float z_far);
	//void SetModelViewMatrix(const DirectX::XMMATRIX& model_view_mx);

	void AddTexturetoSRV(char* name, int width, int height, int bits, unsigned char* data, int texNum, bool mipmap);
	void UpdateTextureInSRV(int width, int height, int xOffset, int yOffset, int bits, unsigned char* data, int texNum);
	void CreateSkyBoxSRV();

	void AddCustomTextureToSrv(char* path, int texNum, bool mipmap);

	void IterativeUpdateSkyBoxSrv(int width, int height, int bits, unsigned char* data);

	void DeleteTextureFromSRV(int texNum);

	void Test(char* name, int width, int height, int bits, unsigned char* data, int type);

	//void InitMatrix(int widht, int height);

	//DirectX::XMMATRIX GetOrthogonal();
	//DirectX::XMMATRIX GetPerspective();
	//DirectX::XMMATRIX GetModelView();

	void Clear();

	void Swap();

	/*void SetDepthBuffer() {
		context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
	}

	void UnSetDepthBuffer() {
		context->OMSetRenderTargets(1, &render_target_view, nullptr);
	}*/

	void Bind(int texture_index, int textureSlot);
	void BindSkyBox();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	IDXGISwapChain* GetSwapChain();
	ID3D11RenderTargetView* GetRenderTargetView(unsigned int num);
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

	ID3D11DepthStencilView* GetDepthStencilView(int index) {
		return depth_stencil_view[index];
	}

	HWND GetWindow() {
		return window->GetWindow();
	}
};