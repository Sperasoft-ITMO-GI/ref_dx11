#include <Window.h>

Window::Window()
	: instance(nullptr), wndproc(nullptr),
	  width(800), height(600), is_fullscreen(false) {
}

Window::~Window() {
	if (window)
	{
		DestroyWindow(window);
		window = NULL;
	}

	UnregisterClassA(window_class_name, instance);
}

bool Window::Initialize(const HINSTANCE inst, const WNDPROC proc) {
	WNDCLASSA wc;
	wc.style = 0;
	wc.lpfnWndProc = proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = inst;
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(0, IDC_ARROW);;
	wc.hbrBackground = (HBRUSH)COLOR_GRAYTEXT;
	wc.lpszMenuName = 0;
	wc.lpszClassName = window_class_name;

	if (!RegisterClassA(&wc)) {
		printf("[DX11]: RegisterClass Failed");
		MessageBoxA(0, "RegisterClass Failed.", 0, 0);
		return false;
	}

	int	stylebits = 0;
	int	exstyle = 0;

	if (is_fullscreen)
	{
		exstyle = WS_EX_TOPMOST;
		stylebits = WS_POPUP | WS_VISIBLE;
	}
	else
	{
		exstyle = 0;
		stylebits = (WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_VISIBLE);
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, width, height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	if (is_fullscreen)
	{
		posX = 0;
		posY = 0;
	}

	window = CreateWindowExA(	
		exstyle,
		window_class_name, 
		title,
		stylebits,
		posX,
		posY,
		width,
		height,
		NULL,
		NULL,
		inst,
		NULL);
	
	if (!window) {
		con::Outf(L"[DX11]: CreateWindow Failed");
		MessageBoxA(0, "CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	instance = inst;
	wndproc = proc;

	con::Outf(L"[DX11]: Window created\n");

	return true;
}

void Window::SetMode(const int x, const int y, const int w, const int h, const bool fullscreen) {
	width = w;
	height = h;
	is_fullscreen = fullscreen;
	posX = x;
	posY = y;
}

HWND Window::GetWindow() {
	return window;
}

HINSTANCE Window::GetInstance() {
	return instance;
}

uint32_t Window::GetWidth() {
	return width;
}

uint32_t Window::GetHeight() {
	return height;
}

bool Window::IsFullscreen() {
	return is_fullscreen;
}
