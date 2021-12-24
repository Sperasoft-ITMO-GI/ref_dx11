#include "dx11_init.h"

// NOTE: for test loading image only
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>


void InitDX11::DummyTest(char* name, int width, int height, int bits, unsigned char* data, int type)
{

	//it_skin = 0
	//it_sprite = 1
	//it_wall = 2
	//it_pic = 3
	//it_sky = 4

	int lenName = strlen(name);
	char* newName = (char*)malloc(lenName + 1);
	memset(newName, 0, lenName + 1);
	memcpy(newName, name, lenName);
	char* extension = strchr(newName, '.');

	extension[1] = 'p';
	extension[2] = 'n';
	extension[3] = 'g';

	stbi_write_png(newName, width, height, bits / 8, data, width * bits / 8);
	free(newName);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> InitDX11::LoadTestBMP(char* fileName)
{
	using Microsoft::WRL::ComPtr;

	int x;
	int y;
	int c;

	unsigned char* image = stbi_load(fileName , &x, &y, &c, NULL);

	ComPtr<ID3D11ShaderResourceView> texSRV;

	static const uint32_t s_pixel = 0xffc99aff;

	D3D11_SUBRESOURCE_DATA initData = { image, x * c, 0 };

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = x;
	desc.Height = y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> tex;
	HR(d3dDevice->CreateTexture2D(&desc, &initData, tex.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	
	HR(d3dDevice->CreateShaderResourceView(tex.Get(),
		&SRVDesc, texSRV.GetAddressOf()));

	//STBI_FREE(image);

	return texSRV;
}

bool InitDX11::CompileQuadShaders()
{
	using Microsoft::WRL::ComPtr;

	// Read quad texture shaders from files
	//ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3DBlob> vertex_blob;
	HR(D3DCompileFromFile(L"ref_dx11\\shaders\\QuadTexture.hlsl", NULL, NULL, "VSmain", "vs_5_0", NULL, NULL, &vertex_blob, NULL));
	HR(d3dDevice->CreateVertexShader(vertex_blob->GetBufferPointer(), vertex_blob->GetBufferSize(), nullptr, &vertex_PosColTex_shader));

	//ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3DBlob> pixel_blob;
	HR(D3DCompileFromFile(L"ref_dx11\\shaders\\QuadTexture.hlsl", NULL, NULL, "PSmain", "ps_5_0", NULL, NULL, &pixel_blob, NULL));
	HR(d3dDevice->CreatePixelShader(pixel_blob->GetBufferPointer(), pixel_blob->GetBufferSize(), nullptr, &pixel_PosColTex_shader));

	// Set input layout
	//ComPtr<ID3D11InputLayout> input_layout = NULL;
	D3D11_INPUT_ELEMENT_DESC input_element_descriptor[] = {
		{
			"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};

	HR(d3dDevice->CreateInputLayout(
		input_element_descriptor, 3,
		vertex_blob->GetBufferPointer(), vertex_blob->GetBufferSize(),
		&input_PosColTex_layout));

	return true;
}


bool InitDX11::CompileAllShaders()
{
	return true;
}

bool InitDX11::InitializeWindow(HINSTANCE hinstance_, WNDPROC wndProc_)
{
	WNDCLASSA wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = wndProc_;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance_;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_GRAYTEXT;
	wc.lpszMenuName = 0;
	wc.lpszClassName = windowClassName;

	if (!RegisterClassA(&wc))
	{
		MessageBoxA(0, "RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mainWindowWidth, mainWindowHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mainWindow = CreateWindowExA(0, windowClassName, windowName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hinstance_, 0);
	if (!mainWindow)
	{
		MessageBoxA(0, "CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mainWindow, SW_SHOW);
	UpdateWindow(mainWindow);

	hInstance = hinstance_;
	wndProc = wndProc_;

	printf("[DX11]: Window created\n");

	return true;
}


void InitDX11::DrawColored2D(std::array<std::pair<float, float>, 4> vertexes, std::array<float, 4> color) {
	using DirectX::XMFLOAT2;
	using DirectX::XMFLOAT4;
	using Microsoft::WRL::ComPtr;

	// Set vertices from DrawFill function
	std::array<Vertex, 4> vertices; 
	for (auto i = 0; i < vertices.size(); ++i) {
		vertices[i] = {
			XMFLOAT2(vertexes[i].first, vertexes[i].second),
			XMFLOAT4(color.data())
		};
	}

	// Create Vertex Buffer
	ComPtr<ID3D11Buffer> vertex_buffer;

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.CPUAccessFlags = 0u;
	buffer_desc.MiscFlags = 0u;
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = vertices.data();

	HR(d3dDevice->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer));

	// Create Index Buffer
	const unsigned short indices[] = {
		0, 2, 3,
		3, 1, 0
	};

	ComPtr<ID3D11Buffer> index_buffer;

	D3D11_BUFFER_DESC index_buffer_desc{};
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.CPUAccessFlags = 0u;
	index_buffer_desc.MiscFlags = 0u;
	index_buffer_desc.ByteWidth = sizeof(indices);
	index_buffer_desc.StructureByteStride = sizeof(unsigned short); // должно быть ноль на том что не SRV

	D3D11_SUBRESOURCE_DATA index_subresource_data{};
	index_subresource_data.pSysMem = indices;

	HR(d3dDevice->CreateBuffer(&index_buffer_desc, &index_subresource_data, &index_buffer));
	
	// Create constant buffer
	ConstantBufferStr cb = { DirectX::XMMatrixTranspose(orthogonal) };

	ComPtr<ID3D11Buffer> constant_buffer;

	D3D11_BUFFER_DESC constant_buffer_desc{};
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constant_buffer_desc.MiscFlags = 0u;
	constant_buffer_desc.ByteWidth = sizeof(cb);;
	constant_buffer_desc.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA constant_subresource_data{};
	constant_subresource_data.pSysMem = &cb;

	d3dDevice->CreateBuffer(&constant_buffer_desc, &constant_subresource_data, &constant_buffer);

	// Read pixel and vertex shaders from compiled files
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3DBlob> pixel_blob;
	D3DReadFileToBlob(L"ref_dx11\\shaders\\pixel_shader.cso", &pixel_blob);
	HR(d3dDevice->CreatePixelShader(pixel_blob->GetBufferPointer(), pixel_blob->GetBufferSize(), nullptr, &pixel_shader));
	
	ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3DBlob> vertex_blob;
	D3DReadFileToBlob(L"ref_dx11\\shaders\\vertex_shader.cso", &vertex_blob);
	HR(d3dDevice->CreateVertexShader(vertex_blob->GetBufferPointer(), vertex_blob->GetBufferSize(), nullptr, &vertex_shader));
	
	// Set input layout
	ComPtr<ID3D11InputLayout> input_layout;
	D3D11_INPUT_ELEMENT_DESC input_element_descriptor[] = {
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	HR(d3dDevice->CreateInputLayout(
		input_element_descriptor, std::size(input_element_descriptor),
		vertex_blob->GetBufferPointer(), vertex_blob->GetBufferSize(),
		&input_layout
	));


	// Bind all to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	d3dImmediateContext->IASetInputLayout(input_layout.Get());
	d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dImmediateContext->IASetVertexBuffers(0u, 1u, vertex_buffer.GetAddressOf(), &stride, &offset);
	d3dImmediateContext->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
	d3dImmediateContext->VSSetConstantBuffers(0u, 1u, constant_buffer.GetAddressOf());
	d3dImmediateContext->VSSetShader(vertex_shader.Get(), 0, 0);
	d3dImmediateContext->PSSetShader(pixel_shader.Get(), 0, 0);

	d3dImmediateContext->DrawIndexed(std::size(indices), 0u, 0u);
}

// TODO: VSYNC
void InitDX11::SwapBuffers()
{
	swapChain->Present(1, 0);
}

void InitDX11::CreateConstantVar()
{
	orthogonal = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, mainWindowWidth, mainWindowHeight, 0.0f, 0.0f, 100.0f); // (mainWindowWidth, 0.0f, mainWindowHeight, 0.0f, 0.0f, 1.0f)
																														// (0.0f, mainWindowWidth, 0.0f, mainWindowHeight, 0.0f, 1.0f)

	// Create a sampler state for texture sampling in the pixel shader
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	d3dDevice->CreateSamplerState(&samplerDesc, &d3dSamplerState);
}


bool InitDX11::InitializeDX11(HINSTANCE hinstance_, WNDPROC wndProc_)
{
	// creating a window
	InitializeWindow(hinstance_, wndProc_);

	// Create the device and device context.

	// setting default values (in further may be changed)
	d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	enable4xMsaa = false;
	msaaQuality = 0;

	ZeroMemory(&screenViewport, sizeof(D3D11_VIEWPORT));

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


	// 1. Create device context

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		d3dDriverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&d3dDevice,
		&featureLevel,
		&d3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBoxA(0, "D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	// 1.1 Check feature level

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBoxA(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// 2. Check MSAA support and quality level

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	HR(d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality));
	assert(msaaQuality > 0);

	// 3. Describe SWAP_CHAIN

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

	sd.BufferDesc.Width = mainWindowWidth;
	sd.BufferDesc.Height = mainWindowHeight;
	sd.BufferDesc.RefreshRate = QueryRefreshRate(mainWindowWidth, mainWindowHeight, true);
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	sd.SampleDesc.Count = enable4xMsaa ? 4 : 1;
	sd.SampleDesc.Quality = enable4xMsaa ? (msaaQuality - 1) : 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = mainWindow;
	sd.Windowed = fullscreen ? false : true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// 4. Create SWAP_CHAIN instance

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(d3dDevice, &sd, &swapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	assert(d3dImmediateContext);
	assert(d3dDevice);
	assert(swapChain);

	// 5. Create the render target view
	ID3D11Texture2D* backBuffer;
	HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&backBuffer)));
	HR(d3dDevice->CreateRenderTargetView(backBuffer, 0, &renderTargetView));
	ReleaseCOM(backBuffer);

	// 6. Create the depth/stencil buffer

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = mainWindowWidth;
	depthStencilDesc.Height = mainWindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	depthStencilDesc.SampleDesc.Count = enable4xMsaa ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = enable4xMsaa ? (msaaQuality - 1) : 0;

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
	HR(d3dDevice->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView));

	// 7. Bind the render target view

	// Bind the render target view and depth/stencil view to the pipeline.

	d3dImmediateContext->OMSetRenderTargets(1, &renderTargetView, NULL);

	// 8. Set the viewport

	// Set the viewport transform.
	
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = (float)(mainWindowWidth);
	screenViewport.Height = (float)(mainWindowHeight);
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	d3dImmediateContext->RSSetViewports(1, &screenViewport);

	// 9. Setup rasterizer state.
	// NOTE: Current cull mode setted for UI
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state object.
	d3dDevice->CreateRasterizerState(&rasterizerDesc, &d3dRasterizerState);

	d3dImmediateContext->RSSetState(d3dRasterizerState);

	// 10. Creating Blend state

	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.IndependentBlendEnable = false;
	blendStateDesc.RenderTarget[0].BlendEnable =			true;
	blendStateDesc.RenderTarget[0].SrcBlend =				D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend =				D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp =				D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha =			D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha =			D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha =			D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask =	D3D11_COLOR_WRITE_ENABLE_ALL;
	d3dDevice->CreateBlendState(&blendStateDesc, &blendState);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;

	d3dImmediateContext->OMSetBlendState(blendState, blendFactor, sampleMask);

	// Creating constant variables
	// like a orthogonal martrix
	// TODO: need to rename it and move to more appropriate place
	CreateConstantVar();

	// TODO: move this to more appropriate place
	CompileQuadShaders();

	printf("[DX11]: Successfully initialized\n");

	return true;
}

void InitDX11::SetMode(int width_, int height_, bool fullscreen_)
{
	mainWindowWidth = width_;
	mainWindowHeight = height_;
	fullscreen = fullscreen_;
}

void InitDX11::ClearScene()
{
	assert(d3dImmediateContext);

	d3dImmediateContext->ClearRenderTargetView(renderTargetView, DirectX::Colors::DodgerBlue);
	d3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

InitDX11::~InitDX11()
{
	ReleaseCOM(renderTargetView);
	ReleaseCOM(depthStencilView);
	ReleaseCOM(swapChain);
	ReleaseCOM(depthStencilBuffer);
	ReleaseCOM(blendState);
	ReleaseCOM(d3dRasterizerState);

	for (auto i : texArraySRV)
	{
		if (i)
		{
			i.~ComPtr();
		}
	}

	// Restore all default settings.
	if (d3dImmediateContext)
		d3dImmediateContext->ClearState();

	if (d3dDevice != nullptr) 
	{
		if (mainWindow)
		{
			DestroyWindow(mainWindow);
			mainWindow = NULL;
		}

		UnregisterClassA(windowClassName, hInstance);
		printf("%s", "[DX11]: Window destroyed\n");
	}

	ReleaseCOM(d3dImmediateContext);
	ReleaseCOM(d3dDevice);

	printf("%s", "[DX11]: Render destoyed");
}

void InitDX11::AddTexturetoSRV(int width, int height, int bits, unsigned char* data, int texNum, bool dynamic)
{
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
	HR(d3dDevice->CreateTexture2D(&desc, &initData, tex.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	HR(d3dDevice->CreateShaderResourceView(tex.Get(), &SRVDesc, texArraySRV[texNum].GetAddressOf()));
}

void InitDX11::UpdateTextureInSRV(int width, int height, int bits, unsigned char* data, int texNum)
{
	using Microsoft::WRL::ComPtr;

	ID3D11Resource* res = nullptr;
	texArraySRV[texNum].Get()->GetResource(&res);

	d3dImmediateContext->UpdateSubresource(res, NULL, NULL, data, width * (bits / 8), 0);

	ReleaseCOM(res);
}


void InitDX11::DummyDrawingPicture(Vertex_PosTexCol_Info* v, int texNum)
{
	using DirectX::XMFLOAT2;
	using DirectX::XMFLOAT3;
	using DirectX::XMFLOAT4;
	using DirectX::XMVECTOR;
	using DirectX::XMMATRIX;
	using Microsoft::WRL::ComPtr;

	Vertex_PosTexCol vertices[4] = {};

	vertices[0].pos = XMFLOAT2(v->pos.x, v->pos.y);
	vertices[0].texCoord = XMFLOAT2(v->tex.xl, v->tex.yt);
	vertices[0].col = XMFLOAT4(v->col.x, v->col.y, v->col.z, v->col.w);

	vertices[1].pos = XMFLOAT2(v->pos.x + v->pos.width, v->pos.y);
	vertices[1].texCoord = XMFLOAT2(v->tex.xr, v->tex.yt);
	vertices[1].col = XMFLOAT4(v->col.x, v->col.y, v->col.z, v->col.w);

	vertices[2].pos = XMFLOAT2(v->pos.x + v->pos.width, v->pos.y + v->pos.height);
	vertices[2].texCoord = XMFLOAT2(v->tex.xr, v->tex.yb);
	vertices[2].col = XMFLOAT4(v->col.x, v->col.y, v->col.z, v->col.w);

	vertices[3].pos = XMFLOAT2(v->pos.x, v->pos.y + v->pos.height);
	vertices[3].texCoord = XMFLOAT2(v->tex.xl, v->tex.yb);
	vertices[3].col = XMFLOAT4(v->col.x, v->col.y, v->col.z, v->col.w);

	// Create Vertex Buffer
	ComPtr<ID3D11Buffer> vertex_buffer;

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.StructureByteStride = sizeof(Vertex_PosTexCol); // 0?????

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = vertices;

	HR(d3dDevice->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer));

	// Create Index Buffer
	unsigned short indices[] = {
		2, 1, 0,
		0, 3, 2
	};

	ComPtr<ID3D11Buffer> index_buffer;

	D3D11_BUFFER_DESC index_buffer_desc{};
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.CPUAccessFlags = 0;
	index_buffer_desc.MiscFlags = 0;
	index_buffer_desc.ByteWidth = sizeof(indices);
	index_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA index_subresource_data{};
	index_subresource_data.pSysMem = indices;

	HR(d3dDevice->CreateBuffer(&index_buffer_desc, &index_subresource_data, &index_buffer));

	// Create constant buffer
	XMMATRIX transform = DirectX::XMMatrixTranspose(orthogonal);

	ComPtr<ID3D11Buffer> constant_buffer;

	D3D11_BUFFER_DESC constant_buffer_desc{};
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constant_buffer_desc.MiscFlags = 0;
	constant_buffer_desc.ByteWidth = sizeof(transform);;
	constant_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA constant_subresource_data{};
	constant_subresource_data.pSysMem = &transform;

	d3dDevice->CreateBuffer(&constant_buffer_desc, &constant_subresource_data, &constant_buffer);

	// Bind all to pipeline
	const UINT stride = sizeof(Vertex_PosTexCol);
	const UINT offset = 0;
	d3dImmediateContext->IASetInputLayout(input_PosColTex_layout.Get());
	d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dImmediateContext->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	d3dImmediateContext->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	d3dImmediateContext->PSSetSamplers(0, 1, d3dSamplerState.GetAddressOf());
	d3dImmediateContext->PSSetShaderResources(0, 1, texArraySRV[texNum].GetAddressOf());

	d3dImmediateContext->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
	d3dImmediateContext->VSSetShader(vertex_PosColTex_shader.Get(), 0, 0);
	d3dImmediateContext->PSSetShader(pixel_PosColTex_shader.Get(), 0, 0);

	d3dImmediateContext->DrawIndexed(6, 0, 0);
}
