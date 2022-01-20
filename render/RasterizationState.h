#pragma once

#include <unordered_map>

#include "WndDxIncludes.h"
#include "Renderer.h"

enum RasterizationState {
	CULL_NONE,
	CULL_BACK,
	CULL_FRONT,
};

Microsoft::WRL::ComPtr<ID3D11RasterizerState> MakeCullNoneRS();
Microsoft::WRL::ComPtr<ID3D11RasterizerState> MakeCullBackRS();
Microsoft::WRL::ComPtr<ID3D11RasterizerState> MakeCullFrontRS();

