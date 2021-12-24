#pragma once

#include <memory>
#include <tuple>

#include <WndDxIncludes.h>
#include <Utils.h>
#include <Window.h>

// for now it will be a singletone with base dx11 structured reailized

class Renderer {
public:
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	~Renderer();

	static Renderer* GetInstance();

	bool Initialize(const HINSTANCE instance, const WNDPROC wndproc);

public:
	void SetWindowMode(const uint32_t w, const uint32_t h, const bool fullscreen);

	void AddTexturetoSRV(int width, int height, int bits, unsigned char* data, int texNum, bool dynamic);
	void UpdateTextureInSRV(int width, int height, int bits, unsigned char* data, int texNum);

	void Bind(int texture_index);

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext();
	Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView();
	std::tuple<float, float> GetWindowParameters();

private:
	Renderer();

private:
	std::unique_ptr<Window> window;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	D3D11_VIEWPORT viewport;

	D3D_DRIVER_TYPE driver_type;
	UINT msaa_quality;
	bool is_4xmsaa_enable;
	
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_array_srv[1600];

private:
	static Renderer* renderer;
};