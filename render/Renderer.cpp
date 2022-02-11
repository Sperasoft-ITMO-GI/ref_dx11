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
	  is_4xmsaa_enable(false),
      is_initialized(false),
      sampler(nullptr), 
	  sky_box_view(nullptr),
	  model_view(DirectX::XMMatrixIdentity()),
      orthogonal(DirectX::XMMatrixIdentity()), 
      perspective(DirectX::XMMatrixIdentity()){
}

Renderer* Renderer::GetInstance() {
	if (renderer == nullptr) {
		renderer = new Renderer();
	}

	return renderer;
}

bool Renderer::Initialize(const HINSTANCE instance, const WNDPROC wndproc) {
	if (!is_initialized) {
		using Microsoft::WRL::ComPtr;

		window->Initialize(instance, wndproc);

		UINT debug_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		debug_flags |= D3D11_CREATE_DEVICE_DEBUG;
#else
		debug_flags |= NULL;
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

		IDXGIDevice* dxgi_device;
		IDXGIAdapter* adapter;
		IDXGIFactory* factory;
		DXCHECK(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device));
		DXCHECK(dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter));
		DXCHECK(adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory));
		DXCHECK(factory->CreateSwapChain(device, &swap_chain_desc, &swap_chain));

		dxgi_device->Release();
		adapter->Release();
		factory->Release();

		// DO we really need it?
		assert(device);
		assert(context);
		assert(swap_chain);

		// It is default render target view 
	    // Do we need it there???
		ID3D11Texture2D* back_buffer;
		DXCHECK(swap_chain->GetBuffer(0u, __uuidof(ID3D11Texture2D), (void**)&back_buffer));
		DXCHECK(device->CreateRenderTargetView(back_buffer, 0, &render_target_view));
		back_buffer->Release();

		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		// Set up the description of the stencil state.
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // только дл€ неба такой
		DXCHECK(device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state));

		D3D11_TEXTURE2D_DESC depth_stencil_tex;
		ZeroMemory(&depth_stencil_tex, sizeof(D3D11_TEXTURE2D_DESC));
		depth_stencil_tex.Width = window->GetWidth();
		depth_stencil_tex.Height = window->GetHeight();
		depth_stencil_tex.MipLevels = 1;
		depth_stencil_tex.ArraySize = 1;
		depth_stencil_tex.Format = DXGI_FORMAT_D32_FLOAT;
		depth_stencil_tex.SampleDesc.Count = IsMsaaEnable() ? 4 : 1;
		depth_stencil_tex.SampleDesc.Quality = IsMsaaEnable() ? (GetMSAAQuality() - 1) : 0;
		depth_stencil_tex.Usage = D3D11_USAGE_DEFAULT;
		depth_stencil_tex.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depth_stencil_tex.CPUAccessFlags = 0;
		depth_stencil_tex.MiscFlags = 0;

		ID3D11Texture2D* buffer;

		DXCHECK(device->CreateTexture2D(&depth_stencil_tex, 0, &buffer));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
		ZeroMemory(&depth_stencil_view_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0u;

		DXCHECK(device->CreateDepthStencilView(buffer, &depth_stencil_view_desc, &depth_stencil_view));

		buffer->Release();

		context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
		context->OMSetDepthStencilState(depth_stencil_state, 1);

		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT>(window->GetWidth());
		viewport.Height = static_cast<FLOAT>(window->GetHeight());
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		context->RSSetViewports(1, &viewport);

		D3D11_SAMPLER_DESC sampler_desc;
		ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampler_desc.BorderColor[0] = 1.0f;
		sampler_desc.BorderColor[1] = 1.0f;
		sampler_desc.BorderColor[2] = 1.0f;
		sampler_desc.BorderColor[3] = 1.0f;
		sampler_desc.MipLODBias = 0.0f;
		sampler_desc.MinLOD = -FLT_MAX;
		sampler_desc.MaxLOD = FLT_MAX;

		device->CreateSamplerState(&sampler_desc, &sampler);

		context->PSSetSamplers(0, 1, &sampler);

		InitMatrix(width, height);

		is_initialized = true;
		return true;
	}
	return false;
}

void Renderer::SetWindowMode(const int posX, const int posY, const int w, const int h, const bool fullscreen) {
	window->SetMode(posX, posY, w, h, fullscreen);
}

void Renderer::SetPerspectiveMatrix(const float fov, const float aspect_ratio, const float z_near, const float z_far) {
	using namespace DirectX;
	perspective = DirectX::XMMatrixPerspectiveFovRH(XMConvertToRadians(fov), aspect_ratio, z_near, z_far);
}

void Renderer::SetModelViewMatrix(const DirectX::XMMATRIX& model_view_mx) {
	model_view = model_view_mx;
}

void Renderer::AddTexturetoSRV(int width, int height, int bits, unsigned char* data, int texNum, bool mipmap)
{
	if (texNum == 262 || texNum == 263 || texNum == 264 || texNum == 265 || texNum == 266 || texNum == 267) {
		//unsigned char* sky_data = nullptr;
		//std::swap(sky_data, data);
		//sky_box_data[texNum - 262].pSysMem = sky_data;
		//sky_box_data[texNum - 262].SysMemPitch = width * (bits / 8);
		//sky_box_data[texNum - 262].SysMemSlicePitch = 0;
	}
	else {
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = data;
		initData.SysMemPitch = width * (bits / 8);
		initData.SysMemSlicePitch = 0;

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (bits == 8)
			format = DXGI_FORMAT_R8_UNORM;
		else if (bits == 32)
			format = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D11_USAGE usage = D3D11_USAGE_DEFAULT;

		UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;
		UINT miscFlags = 0;
		UINT mipLevelsTexture = 1;
		UINT mipLevelsSRV = 1;
		if (mipmap)
		{
			bindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			miscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			mipLevelsTexture = 0;
			mipLevelsSRV = -1;
		}

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = mipLevelsTexture;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = usage;
		desc.CPUAccessFlags = 0;
		desc.BindFlags = bindFlags;
		desc.MiscFlags = miscFlags;

		ID3D11Texture2D* tex = nullptr;

		if (mipmap)
		{
			DXCHECK(device->CreateTexture2D(&desc, nullptr, &tex));
			context->UpdateSubresource(tex, 0u, NULL, initData.pSysMem, initData.SysMemPitch, 0u);
		}
		else
		{
			DXCHECK(device->CreateTexture2D(&desc, &initData, &tex));
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = mipLevelsSRV;

		DXCHECK(device->CreateShaderResourceView(tex, &SRVDesc, &texture_array_srv[texNum]));

		if (mipmap)
		{
			context->GenerateMips(texture_array_srv[texNum]);
		}
	}
}

void Renderer::UpdateTextureInSRV(int width, int height, int xOffset, int yOffset, int bits, unsigned char* data, int texNum) {

	ID3D11Resource* res = nullptr;
	texture_array_srv[texNum]->GetResource(&res);

	D3D11_BOX box { xOffset, yOffset, 0, xOffset + width, yOffset + height, 1 };

	context->UpdateSubresource(res, NULL, &box, data, width * (bits / 8), 0);

	ReleaseCOM(res);
}

void Renderer::CreateSkyBoxSRV()
{
	//int	skytexorder[6] = { 0,2,1,3,4,5 };
	int w, h, n;
	stbi_set_flip_vertically_on_load(true);
	for (int i = 0; i < 6; ++i) {
		std::string path("pics/sky/unit");
		path += std::to_string(i) + ".png";
		unsigned char* data = stbi_load(path.data(), &w, &h, &n, 0);
		sky_box_data[i].pSysMem = data;
		sky_box_data[i].SysMemPitch = w * n;
		sky_box_data[i].SysMemSlicePitch = 0;
	}


	D3D11_TEXTURE2D_DESC texture_desc = {};
	texture_desc.Width = w;
	texture_desc.Height = h;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 6;
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	// create the texture resource
	ID3D11Texture2D* texture;
	DXCHECK(device->CreateTexture2D(&texture_desc, sky_box_data, &texture));

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texture_desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;

	DXCHECK(device->CreateShaderResourceView(texture, &srvDesc, &sky_box_view));
}

void Renderer::DeleteTextureFromSRV(int texNum)
{
	if (texture_array_srv[texNum])
	{
		texture_array_srv[texNum]->Release();
	}
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
		context->PSSetShaderResources(0, 1, &texture_array_srv[texture_index]);
}

void Renderer::BindSkyBox()
{
	context->PSSetShaderResources(0, 1, &sky_box_view);
}

ID3D11Device* Renderer::GetDevice() {
	return device;
}

ID3D11DeviceContext* Renderer::GetContext() {
	return context;
}

IDXGISwapChain* Renderer::GetSwapChain() {
	return swap_chain;
}

ID3D11RenderTargetView* Renderer::GetRenderTargetView() {
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
	for (int i = 0; i < 1600; ++i) 
	{
		// ¬озможно это и не нужно, так как мы всЄ равно убиваем контекст
		// » к тому же где-то ранее мы уже очищаем, поэтому вылетает иногда тут
		//DeleteTextureFromSRV(i);
	}
	sampler->Release();
	render_target_view->Release();
	depth_stencil_state->Release();
	depth_stencil_view->Release();
	swap_chain->Release();
	context->Release();
	device->Release();

	// destroy window
	window.get()->~Window();
}