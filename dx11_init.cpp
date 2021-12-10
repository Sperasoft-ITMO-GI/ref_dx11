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
	sd.BufferDesc.RefreshRate.Numerator = 60; // 60
	sd.BufferDesc.RefreshRate.Denominator = 1; // 1
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

	swapChain->Present(0, 0);
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