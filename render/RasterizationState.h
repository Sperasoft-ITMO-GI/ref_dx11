#pragma once

#include <unordered_map>

#include "WndDxIncludes.h"
#include "Renderer.h"

enum RasterizationState {
	CULL_NONE
};

Microsoft::WRL::ComPtr<ID3D11RasterizerState> MakeCullNoneRS();

