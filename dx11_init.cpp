#include "dx11_init.h"




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
	RECT R = { 0, 0, width, height };
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

void InitDX11::DrawColored2D(std::array<std::pair<float, float>, 4> vertexes, std::array<float, 3> color) {
	using DirectX::XMFLOAT2;
	using DirectX::XMFLOAT3;
	using Microsoft::WRL::ComPtr;

	std::array<Vertex, 4> vertices;
	for (auto i = 0; i < vertices.size(); ++i) {
		vertices[i] = {
			XMFLOAT2(-vertexes[i].first, -vertexes[i].second),
			//XMFLOAT3(color.data())
		};
	}

	ComPtr<ID3D11Buffer> vertex_buffer;

	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.CPUAccessFlags = 0u;
	buffer_desc.MiscFlags = 0u;
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = vertices.data();

	HR(d3dDevice->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer));
	
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	d3dImmediateContext->IASetVertexBuffers(0u, 1u, vertex_buffer.GetAddressOf(), &stride, &offset);

	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3DBlob> blob;
	D3DReadFileToBlob(L"ref_dx11\\shaders\\pixel_shader.cso", &blob);
	HR(d3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixel_shader));
	d3dImmediateContext->PSSetShader(pixel_shader.Get(), 0, 0);

	ComPtr<ID3D11VertexShader> vertex_shader;
	D3DReadFileToBlob(L"ref_dx11\\shaders\\vertex_shader.cso", &blob);
	HR(d3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertex_shader));
	d3dImmediateContext->VSSetShader(vertex_shader.Get(), 0, 0);

	ComPtr<ID3D11InputLayout> input_layout;
	D3D11_INPUT_ELEMENT_DESC input_element_descriptor[] = {
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	HR(d3dDevice->CreateInputLayout(
		input_element_descriptor, std::size(input_element_descriptor),
		blob->GetBufferPointer(), blob->GetBufferSize(),
		&input_layout
	));

	d3dImmediateContext->IASetInputLayout(input_layout.Get());

	d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	d3dImmediateContext->Draw(std::size(vertices), 0u);
	swapChain->Present(0, 0);
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

	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	// TODO: check VSYNC
	sd.BufferDesc.RefreshRate = QueryRefreshRate(width, height, true);
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

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
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

	d3dImmediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// 8. Set the viewport

	// Set the viewport transform.

	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = (float)(width);
	screenViewport.Height = (float)(height);
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	d3dImmediateContext->RSSetViewports(1, &screenViewport);

	printf("[DX11]: Successfully initialized\n");

	return true;
}



void InitDX11::SetMode(int width_, int height_, bool fullscreen_)
{
	width = width_;
	height = height_;
	fullscreen = fullscreen_;
}

void InitDX11::DrawScene()
{
	assert(d3dImmediateContext);
	assert(swapChain);

	d3dImmediateContext->ClearRenderTargetView(renderTargetView, DirectX::Colors::LightBlue);
	d3dImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//swapChain->Present(0, 0);
}

InitDX11::~InitDX11()
{
	ReleaseCOM(renderTargetView);
	ReleaseCOM(depthStencilView);
	ReleaseCOM(swapChain);
	ReleaseCOM(depthStencilBuffer);

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

DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync)
{
	DXGI_RATIONAL refreshRate = { 0, 1 };
	if (vsync)
	{
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		DXGI_MODE_DESC* displayModeList;

		// Create a DirectX graphics interface factory.
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Could not create DXGIFactory instance."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to create DXGIFactory.");
		}

		hr = factory->EnumAdapters(0, &adapter);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapters."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapters.");
		}

		hr = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapter outputs."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapter outputs.");
		}

		UINT numDisplayModes;
		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
		}

		displayModeList = new DXGI_MODE_DESC[numDisplayModes];
		assert(displayModeList);

		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
		}

		// Now store the refresh rate of the monitor that matches the width and height of the requested screen.
		for (UINT i = 0; i < numDisplayModes; ++i)
		{
			printf("Widht: %d, Height: %d\n", displayModeList[i].Width, displayModeList[i].Height);
			if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
			{
				refreshRate = displayModeList[i].RefreshRate;
			}
		}

		delete[] displayModeList;
		ReleaseCOM(adapterOutput);
		ReleaseCOM(adapter);
		ReleaseCOM(factory);
	}

	return refreshRate;
}
