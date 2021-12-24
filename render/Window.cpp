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
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = inst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_GRAYTEXT;
	wc.lpszMenuName = 0;
	wc.lpszClassName = window_class_name;

	if (!RegisterClassA(&wc)) {
		printf("[DX11]: RegisterClass Failed");
		MessageBoxA(0, "RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, width, height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	window = CreateWindowExA(0, window_class_name, title,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, inst, 0);
	if (!window) {
		printf("[DX11]: CreateWindow Failed");
		MessageBoxA(0, "CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	instance = inst;
	wndproc = proc;

	printf("[DX11]: Window created\n");

	return true;
}

void Window::SetMode(const uint32_t w, const uint32_t h, const bool fullscreen) {
	width = w;
	height = h;
	is_fullscreen = fullscreen;
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
