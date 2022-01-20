#pragma once

#include <array>
#include <unordered_map>

#include "WndDxIncludes.h"
#include "Renderer.h"

enum BlendState {
	ALPHA,
	NOALPHA,
};

Microsoft::WRL::ComPtr<ID3D11BlendState> MakeAlphaBS();

