#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

using std::make_unique;
using Microsoft::WRL::ComPtr;

Renderer* Renderer::renderer = nullptr;

Renderer::Renderer()
	: window(make_unique<Window>()),
	  device(nullptr),
	  context(nullptr),
	  swap_chain(nullptr),
	  render_target_view(nullptr),
	  viewport({ 0 }),
	  driver_type(D3D_DRIVER_TYPE_HARDWARE),
	  msaa_quality(0u),
	  is_4xmsaa_enable(false) {
}

Renderer* Renderer::GetInstance() {
	if (renderer == nullptr) {
		renderer = new Renderer();
	}

	return renderer;
}

bool Renderer::Initialize(const HINSTANCE instance, const WNDPROC wndproc) {
	using Microsoft::WRL::ComPtr;

	window->Initialize(instance, wndproc);

	UINT debug_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	debug_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature_level;
	DXCHECK(
		D3D11CreateDevice(
			nullptr, driver_type, 0,
			debug_flags,
			nullptr, 0,
			D3D11_SDK_VERSION,
			&device,
			&feature_level,
			&context
		)
	);

	if (feature_level != D3D_FEATURE_LEVEL_11_0) {
		printf("[DX11]: Direct3D Feature Level 11 unsupported.");
		MessageBoxA(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
		//return false;
	}

	DXCHECK(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaa_quality));
	assert(msaa_quality > 0);

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	const float width = window->GetWidth();
	const float height = window->GetHeight();

	swap_chain_desc.BufferDesc.Width = width;
	swap_chain_desc.BufferDesc.Height = height;
	swap_chain_desc.BufferDesc.RefreshRate = QueryRefreshRate(width, height, true);
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SampleDesc.Count = is_4xmsaa_enable ? 4 : 1;
	swap_chain_desc.SampleDesc.Quality = is_4xmsaa_enable ? (msaa_quality - 1) : 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = window->GetWindow();
	swap_chain_desc.Windowed = window->IsFullscreen() ? false : true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGIDevice> dxgi_device;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIFactory> factory;
	DXCHECK(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device));
	DXCHECK(dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter));
	DXCHECK(adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory));
	DXCHECK(factory->CreateSwapChain(device.Get(), &swap_chain_desc, &swap_chain));

	// DO we really need it?
	assert(device);
	assert(context);
	assert(swap_chain);

	D3D11_TEXTURE2D_DESC depth_stencil_view_desc;
	ZeroMemory(&depth_stencil_view_desc, sizeof(D3D11_TEXTURE2D_DESC));
	depth_stencil_view_desc.Width = window->GetWidth();
	depth_stencil_view_desc.Height = window->GetHeight();
	depth_stencil_view_desc.MipLevels = 1;
	depth_stencil_view_desc.ArraySize = 1;
	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.SampleDesc.Count = IsMsaaEnable() ? 4 : 1;
	depth_stencil_view_desc.SampleDesc.Quality = IsMsaaEnable() ? (GetMSAAQuality() - 1) : 0;
	depth_stencil_view_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_view_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_view_desc.CPUAccessFlags = 0;
	depth_stencil_view_desc.MiscFlags = 0;

	ID3D11Texture2D* buffer;

	DXCHECK(device->CreateTexture2D(&depth_stencil_view_desc, 0, &buffer));
	DXCHECK(device->CreateDepthStencilView(buffer, 0, &depth_stencil_view));

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.StencilEnable = FALSE;

	DXCHECK(device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state));

	// It is default render target view 
	// Do we need it there???
	ID3D11Texture2D* back_buffer;
	DXCHECK(swap_chain->GetBuffer(0u, __uuidof(ID3D11Texture2D), (void**)&back_buffer));
	DXCHECK(device->CreateRenderTargetView(back_buffer, 0, &render_target_view));
	back_buffer->Release();

	context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
	context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT>(window->GetWidth());
	viewport.Height = static_cast<FLOAT>(window->GetHeight());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MaxAnisotropy = 1;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.BorderColor[0] = 1.0f;
	sampler_desc.BorderColor[1] = 1.0f;
	sampler_desc.BorderColor[2] = 1.0f;
	sampler_desc.BorderColor[3] = 1.0f;
	sampler_desc.MinLOD = -FLT_MAX;
	sampler_desc.MaxLOD = FLT_MAX;

	device->CreateSamplerState(&sampler_desc, &sampler);

	context->PSSetSamplers(0, 1, sampler.GetAddressOf());

	InitMatrix(width, height);

	return true;
}

void Renderer::SetWindowMode(const uint32_t w, const uint32_t h, const bool fullscreen) {
	window->SetMode(w, h, fullscreen);
}

void Renderer::SetPerspectiveMatrix(const float fov, const float aspect_ratio, const float z_near, const float z_far) {
	using namespace DirectX;
	perspective = DirectX::XMMatrixPerspectiveFovRH(XMConvertToRadians(fov), aspect_ratio, z_near, z_far);
}

void Renderer::SetModelViewMatrix(const DirectX::XMMATRIX& model_view_mx) {
	model_view = model_view_mx;
}

void Renderer::AddTexturetoSRV(int width, int height, int bits, unsigned char* data, int texNum, bool dynamic) {
	using DirectX::XMFLOAT2;
	using DirectX::XMFLOAT3;
	using DirectX::XMFLOAT4;
	using Microsoft::WRL::ComPtr;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = width * (bits / 8);
	initData.SysMemSlicePitch = 0;

	DXGI_FORMAT format;
	if (bits == 8)
		format = DXGI_FORMAT_R8_UNORM;
	else if (bits == 32)
		format = DXGI_FORMAT_R8G8B8A8_UNORM;

	D3D11_USAGE usage;
	if (dynamic)
		usage = D3D11_USAGE_DEFAULT;
	else
		usage = D3D11_USAGE_IMMUTABLE;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = usage;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> tex;
	DXCHECK(device->CreateTexture2D(&desc, &initData, tex.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	DXCHECK(device->CreateShaderResourceView(tex.Get(), &SRVDesc, texture_array_srv[texNum].GetAddressOf()));
}

void Renderer::UpdateTextureInSRV(int width, int height, int bits, unsigned char* data, int texNum) {
	using Microsoft::WRL::ComPtr;

	ID3D11Resource* res = nullptr;
	texture_array_srv[texNum].Get()->GetResource(&res);

	context->UpdateSubresource(res, NULL, NULL, data, width * (bits / 8), 0);

	ReleaseCOM(res);
}

void Renderer::DeleteTextureFromSRV(int texNum)
{
	texture_array_srv[texNum].Get()->Release();
}

void Renderer::Test(char* name, int width, int height, int bits, unsigned char* data, int type) 
{
	/*char* newName = (char*)malloc(256);
	memset(newName, 0, 256);
	strcpy(newName, "pics/");

	char* nameAfterSlash = strrchr(name, '/') + 1;

	if ((nameAfterSlash == NULL) || (name[0] == '*'))
	{
		nameAfterSlash = name;
	}

	switch (type)
	{
		case 0:
			strcat(newName, "skin/");
			break;
		case 1:
			strcat(newName, "sprite/");
			break;
		case 2:
			strcat(newName, "wall/");
			break;
		case 3:
			strcat(newName, "pic/");
			break;
		case 4:
			strcat(newName, "sky/");
			break;
	}

	strcat(newName, nameAfterSlash);
	strcat(newName, ".png\0");

	stbi_write_png(newName, width, height, bits / 8, data, width * bits / 8);
	free(newName);*/
}

void Renderer::Bind(int texture_index) {
	if(texture_index != -1)
		context->PSSetShaderResources(0, 1, texture_array_srv[texture_index].GetAddressOf());
}

ComPtr<ID3D11Device> Renderer::GetDevice() {
	return device;
}

ComPtr<ID3D11DeviceContext> Renderer::GetContext() {
	return context;
}

ComPtr<IDXGISwapChain> Renderer::GetSwapChain() {
	return swap_chain;
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> Renderer::GetRenderTargetView() {
	return render_target_view;
}

std::tuple<float, float> Renderer::GetWindowParameters() {
	return { static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()) };
}

void Renderer::InitMatrix(int width, int height)
{
	const float PI_32 = 3.14159265358979323846f;
	orthogonal = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, 0.0f, 1000.0f);
}

DirectX::XMMATRIX Renderer::GetOrthogonal()
{
	return orthogonal;
}

DirectX::XMMATRIX Renderer::GetPerspective()
{
	return perspective;
}

DirectX::XMMATRIX Renderer::GetModelView()
{
	return model_view;
}

Renderer::~Renderer()
{
	// release SRV's
	

	// destroy window
	window.get()->~Window();
}