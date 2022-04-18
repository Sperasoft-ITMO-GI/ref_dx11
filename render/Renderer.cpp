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
	  //render_target_view(),
	  viewport({ 0 }),
	  driver_type(D3D_DRIVER_TYPE_HARDWARE),
	  msaa_quality(0u),
	  is_4xmsaa_enable(false),
      is_initialized(false),
      sampler(nullptr), 
	  sky_box_view(nullptr){}

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

		HRESULT hr = context->QueryInterface(__uuidof(perf), reinterpret_cast<void**>(&perf));
		if (FAILED(hr))
			return false;

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
		swap_chain_desc.BufferCount = 2;
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
		// Creating main Render target view
		ID3D11Texture2D* back_buffer;
		DXCHECK(swap_chain->GetBuffer(0u, __uuidof(ID3D11Texture2D), (void**)&back_buffer));
		DXCHECK(device->CreateRenderTargetView(back_buffer, 0, &render_target_views[EffectsRTV::BACKBUFFER]));

		back_buffer->Release();

		D3D11_TEXTURE2D_DESC texture_desc;
		ZeroMemory(&texture_desc, sizeof(D3D11_TEXTURE2D_DESC));
		texture_desc.Width = width;
		texture_desc.Height = height;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_desc.SampleDesc.Count = 1;
		texture_desc.Usage = D3D11_USAGE_DEFAULT;
		texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture_desc.CPUAccessFlags = 0;
		texture_desc.MiscFlags = 0;

		for (int i = 0; i < render_textures_count - 7; ++i) {
			DXCHECK(device->CreateTexture2D(&texture_desc, nullptr, &render_textures[i]));
		}

		texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		DXCHECK(device->CreateTexture2D(&texture_desc, nullptr, &render_textures[LIGHTMAP_SRV]));

		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
		ZeroMemory(&render_target_view_desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		render_target_view_desc.Format = texture_desc.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		render_target_view_desc.Texture2D.MipSlice = 0;

		for (int i = 1; i < render_targets_count - 4; ++i) {
			DXCHECK(device->CreateRenderTargetView(render_textures[i - 1], &render_target_view_desc, &render_target_views[i]));
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
		ZeroMemory(&shader_resource_view_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shader_resource_view_desc.Format = texture_desc.Format;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
		shader_resource_view_desc.Texture2D.MipLevels = 1;

		for (int i = 0; i < shader_resource_views_count - 6; ++i) {
			DXCHECK(device->CreateShaderResourceView(
				render_textures[i],
				&shader_resource_view_desc,
				&shader_resource_views[i])
			);
		}

		texture_desc.Format = DXGI_FORMAT_R16G16_FLOAT;
		render_target_view_desc.Format = texture_desc.Format;
		shader_resource_view_desc.Format = texture_desc.Format;
		DXCHECK(device->CreateTexture2D(&texture_desc, nullptr, &render_textures[EffectsSRV::VELOCITY_SRV]));
		DXCHECK(device->CreateRenderTargetView(render_textures[EffectsSRV::VELOCITY_SRV],
			&render_target_view_desc, &render_target_views[EffectsRTV::VELOSITY]));
		DXCHECK(device->CreateShaderResourceView(
			render_textures[EffectsSRV::VELOCITY_SRV],
			&shader_resource_view_desc,
			&shader_resource_views[EffectsSRV::VELOCITY_SRV])
		);		
		DXCHECK(device->CreateTexture2D(&texture_desc, nullptr, &render_textures[EffectsSRV::VELOCITY_HIST_SRV]));
		DXCHECK(device->CreateRenderTargetView(render_textures[EffectsSRV::VELOCITY_HIST_SRV],
			&render_target_view_desc, &render_target_views[EffectsRTV::VELOSITY_HIST]));
		DXCHECK(device->CreateShaderResourceView(
			render_textures[EffectsSRV::VELOCITY_HIST_SRV],
			&shader_resource_view_desc,
			&shader_resource_views[EffectsSRV::VELOCITY_HIST_SRV])
		);

		texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		render_target_view_desc.Format = texture_desc.Format;
		shader_resource_view_desc.Format = texture_desc.Format;
		DXCHECK(device->CreateTexture2D(&texture_desc, nullptr, &render_textures[POSITIONS_SRV]));
		DXCHECK(device->CreateRenderTargetView(render_textures[EffectsSRV::POSITIONS_SRV],
			&render_target_view_desc, &render_target_views[EffectsRTV::POSITIONS]));
		DXCHECK(device->CreateShaderResourceView(
			render_textures[EffectsSRV::POSITIONS_SRV],
			&shader_resource_view_desc,
			&shader_resource_views[EffectsSRV::POSITIONS_SRV])
		);

		texture_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		render_target_view_desc.Format = texture_desc.Format;
		shader_resource_view_desc.Format = texture_desc.Format;
		DXCHECK(device->CreateTexture2D(&texture_desc, nullptr, &render_textures[NORMALS_SRV]));
		DXCHECK(device->CreateRenderTargetView(render_textures[EffectsSRV::NORMALS_SRV],
			&render_target_view_desc, &render_target_views[EffectsRTV::NORMALS]));
		DXCHECK(device->CreateShaderResourceView(
			render_textures[EffectsSRV::NORMALS_SRV],
			&shader_resource_view_desc,
			&shader_resource_views[EffectsSRV::NORMALS_SRV])
		);

		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		// Set up the description of the stencil state.
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; 
		DXCHECK(device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state));

		D3D11_TEXTURE2D_DESC depth_stencil_tex;
		ZeroMemory(&depth_stencil_tex, sizeof(D3D11_TEXTURE2D_DESC));
		depth_stencil_tex.Width = window->GetWidth();
		depth_stencil_tex.Height = window->GetHeight();
		depth_stencil_tex.MipLevels = 1;
		depth_stencil_tex.ArraySize = 1;
		depth_stencil_tex.Format = DXGI_FORMAT_R32_TYPELESS;
		depth_stencil_tex.SampleDesc.Count = IsMsaaEnable() ? 4 : 1;
		depth_stencil_tex.SampleDesc.Quality = IsMsaaEnable() ? (GetMSAAQuality() - 1) : 0;
		depth_stencil_tex.Usage = D3D11_USAGE_DEFAULT;
		depth_stencil_tex.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depth_stencil_tex.CPUAccessFlags = 0;
		depth_stencil_tex.MiscFlags = 0;

		//ID3D11Texture2D* buffer;

		DXCHECK(device->CreateTexture2D(&depth_stencil_tex, 0, &render_textures[EffectsSRV::DEPTH_SRV]));
		DXCHECK(device->CreateTexture2D(&depth_stencil_tex, 0, &render_textures[EffectsSRV::DEPTH_HIST_SRV]));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
		ZeroMemory(&depth_stencil_view_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0u;

		DXCHECK(device->CreateDepthStencilView(
			render_textures[EffectsSRV::DEPTH_SRV], 
			&depth_stencil_view_desc, 
			&depth_stencil_view[0])
		);		
		DXCHECK(device->CreateDepthStencilView(
			render_textures[EffectsSRV::DEPTH_HIST_SRV], 
			&depth_stencil_view_desc, 
			&depth_stencil_view[1])
		);

		shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
		DXCHECK(device->CreateShaderResourceView(
			render_textures[EffectsSRV::DEPTH_SRV],
			&shader_resource_view_desc,
			&shader_resource_views[EffectsSRV::DEPTH_SRV])
		);		
		DXCHECK(device->CreateShaderResourceView(
			render_textures[EffectsSRV::DEPTH_HIST_SRV],
			&shader_resource_view_desc,
			&shader_resource_views[EffectsSRV::DEPTH_HIST_SRV])
		);

		//buffer->Release();

		// TODO: replace this to begin frame
		//context->OMSetRenderTargets(renderTargets, render_target_view, depth_stencil_view);
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
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
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
		context->CSSetSamplers(0, 1, &sampler);

		//InitMatrix(width, height);

		is_initialized = true;
		return true;
	}
	return false;
}

void Renderer::SetWindowMode(const int posX, const int posY, const int w, const int h, const bool fullscreen) {
	window->SetMode(posX, posY, w, h, fullscreen);
}

//void Renderer::SetPerspectiveMatrix(const float fov, const float aspect_ratio, const float z_near, const float z_far) {
//	using namespace DirectX;
//	perspective = DirectX::XMMatrixPerspectiveFovRH(XMConvertToRadians(fov), aspect_ratio, z_near, z_far);
//}
//
//void Renderer::SetModelViewMatrix(const DirectX::XMMATRIX& model_view_mx) {
//	model_view = model_view_mx;
//}

void Renderer::AddTexturetoSRV(char* name, int width, int height, int bits, unsigned char* data, int texNum, bool mipmap)
{
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

	if (texNum == 1024) {
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	}


	if (mipmap)
	{
		DXCHECK(device->CreateTexture2D(&desc, nullptr, &tex));
		context->UpdateSubresource(tex, 0u, NULL, initData.pSysMem, initData.SysMemPitch, 0u);
	}
	else
	{
		DXCHECK(device->CreateTexture2D(&desc, &initData, &tex));
	}

	if (texNum == 1024)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
		ZeroMemory(&uav_desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uav_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uav_desc.Texture2D.MipSlice = 0;
		DXCHECK(device->CreateUnorderedAccessView(tex, &uav_desc, &uav));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = mipLevelsSRV;

	DXCHECK(device->CreateShaderResourceView(tex, &SRVDesc, &texture_array_srv[texNum]));

#if defined(DEBUG) || defined(_DEBUG)
	texture_array_srv[texNum]->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
#endif
	char* curr_name = "textures/e1u1/baselt_2.wal";
	if (!strcmp(curr_name, name)) {
		lamp_indexes[0] = texNum;
	}
	curr_name = "textures/e1u1/ceil1_4.wal";
	if (!strcmp(curr_name, name)) {
		lamp_indexes[1] = texNum;
	}
	curr_name = "textures/e1u1/ceil1_8.wal";
	if (!strcmp(curr_name, name)) {
		lamp_indexes[2] = texNum;
	}	
	curr_name = "textures/e1u1/baselt_b.wal";
	if (!strcmp(curr_name, name)) {
		lamp_indexes[3] = texNum;
	}

	if (mipmap)
	{
		context->GenerateMips(texture_array_srv[texNum]);
	}
}

void Renderer::AddCustomTextureToSrv(char* path, int texNum, bool mipmap)
{
	int width;
	int height;
	int comp;
	unsigned char* image = stbi_load(path, &width, &height, &comp, NULL);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = image;
	initData.SysMemPitch = width * comp;
	initData.SysMemSlicePitch = 0;

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

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

	STBI_FREE(image);
}

void Renderer::UpdateTextureInSRV(int width, int height, int xOffset, int yOffset, int bits, unsigned char* data, int texNum) {

	ID3D11Resource* res = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};

	texture_array_srv[texNum]->GetResource(&res);

	D3D11_BOX box { xOffset, yOffset, 0, xOffset + width, yOffset + height, 1 };

	context->UpdateSubresource(res, NULL, &box, data, width * (bits / 8), 0);

	ReleaseCOM(res);
}

void Renderer::CreateSkyBoxSRV()
{

	if (skyBoxIterator > 0)
	{
		sky_box_view->Release();
		skyBoxIterator = 0;

		for (int i = 0; i < 6; ++i)
			delete sky_data[i];

		delete sky_data;
	}

	sky_data = new unsigned char* [6];
	for (int i = 0; i < 6; ++i)
		sky_data[i] = new unsigned char[256 * 256 * 4];

	int width = 256;
	int height = 256;

	D3D11_TEXTURE2D_DESC texture_desc = {};
	texture_desc.Width = width;
	texture_desc.Height = height;
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
	DXCHECK(device->CreateTexture2D(&texture_desc, nullptr, &texture));

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texture_desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;

	DXCHECK(device->CreateShaderResourceView(texture, &srvDesc, &sky_box_view));
}

void Renderer::IterativeUpdateSkyBoxSrv(int width, int height, int bits, unsigned char* data)
{
	for (int i = 0; i < width * height * 4; ++i) {
		sky_data[skyBoxIterator][i] = *(data + i);
	}

	skyBoxIterator++;
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

void Renderer::Bind(int texture_index, int textureSlot) {
	if (texture_index != -1)
	{
		context->PSSetShaderResources(textureSlot, 1, &texture_array_srv[texture_index]);
	}
}

void Renderer::BindSkyBox()
{
	context->PSSetShaderResources(skyboxTexture.slot, 1, &sky_box_view);
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

ID3D11RenderTargetView* Renderer::GetRenderTargetView(unsigned int num) {
	if (num < render_targets_count)
		return render_target_views[num];
	else
		return nullptr;
}

std::tuple<float, float> Renderer::GetWindowParameters() {
	return { static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()) };
}

void Renderer::Clear() {

	/*for (int i = 0; i < render_targets_count; i++) {
		context->ClearRenderTargetView(render_target_views[i], DirectX::Colors::Black);
	}*/

	context->ClearRenderTargetView(render_target_views[BACKBUFFER], DirectX::Colors::Black);

	/*
	BACKBUFFER = 0,
	SCENE,
	SCENE_HIST, 
	LIGHTMAP,
	MASK,
	MOTION_BLUR,
	BLOOM_1,
	BLOOM_2,
	FXAA,
	EFFECT,
	VELOSITY,
	*/

	if (index == 1) {
		context->ClearRenderTargetView(render_target_views[SCENE_HIST], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[LIGHTMAP], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[MASK], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[SCENE], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[BLOOM_1], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[BLOOM_2], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[FXAA], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[EFFECT], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[VELOCITY_SRV], DirectX::Colors::Black);
		//context->ClearRenderTargetView(render_target_views[SCENE_COLOR_SRV], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[POSITIONS_SRV], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[NORMALS_SRV], DirectX::Colors::Black);
		//context->ClearDepthStencilView(depth_stencil_view[1], D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}
	else {
		context->ClearRenderTargetView(render_target_views[SCENE], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[LIGHTMAP], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[MASK], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[MOTION_BLUR], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[BLOOM_1], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[BLOOM_2], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[FXAA], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[EFFECT], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[VELOCITY_SRV], DirectX::Colors::Black);
		//context->ClearRenderTargetView(render_target_views[SCENE_COLOR_SRV], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[POSITIONS_SRV], DirectX::Colors::Black);
		context->ClearRenderTargetView(render_target_views[NORMALS_SRV], DirectX::Colors::Black);
		//context->ClearDepthStencilView(depth_stencil_view[0], D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	context->ClearDepthStencilView(depth_stencil_view[1], D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->ClearDepthStencilView(depth_stencil_view[0], D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Renderer::Swap() {
	swap_chain->Present(1, 0);
}

//void Renderer::InitMatrix(int width, int height)
//{
//	const float PI_32 = 3.14159265358979323846f;
//	orthogonal = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, 0.0f, 1000.0f);
//}
//
//DirectX::XMMATRIX Renderer::GetOrthogonal()
//{
//	return orthogonal;
//}
//
//DirectX::XMMATRIX Renderer::GetPerspective()
//{
//	return perspective;
//}
//
//DirectX::XMMATRIX Renderer::GetModelView()
//{
//	return model_view;
//}

Renderer::~Renderer()
{
	// release SRV's
	for (int i = 0; i < 1600; ++i) 
	{
		// Возможно это и не нужно, так как мы всё равно убиваем контекст
		// И к тому же где-то ранее мы уже очищаем, поэтому вылетает иногда тут
		//DeleteTextureFromSRV(i);
	}
	sampler->Release();

	for (int i = 0; i < render_targets_count; i++) {
		render_target_views[i]->Release();
	}

	depth_stencil_state->Release();
	for (int i = 0; i < 2; ++i)
		depth_stencil_view[i]->Release();
	swap_chain->Release();
	context->Release();
	device->Release();

	// destroy window
	window.get()->~Window();
}

void Renderer::BeginEvent(wchar_t* name)
{
	perf->BeginEvent(name);
}

void Renderer::EndEvent()
{
	perf->EndEvent();
}

void Renderer::SetMarker(wchar_t* name)
{
	perf->SetMarker(name);
}