#pragma once

#include <WndDxIncludes.h>

class Window final {
public:
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;

	Window();
	~Window();

	bool Initialize(const HINSTANCE inst, const WNDPROC proc);
	void SetMode(const uint32_t w, const uint32_t h, const bool fullscreen);

	//void DestroyWindow();

	HWND GetWindow();
	HINSTANCE GetInstance();
	uint32_t GetWidth();
	uint32_t GetHeight();

	bool IsFullscreen();

private:
	HWND window;
	HINSTANCE instance;
	WNDPROC wndproc;

private:
	uint32_t width;
	uint32_t height;

	bool is_fullscreen;

	char* title = "Quake 2 DX11";
	char* window_class_name = "Quake 2";
};