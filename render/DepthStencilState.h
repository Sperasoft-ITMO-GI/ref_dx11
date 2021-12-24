#pragma once

#include <tuple>
#include <unordered_map>

#include "WndDxIncludes.h"
#include "Renderer.h"

enum DepthStencilState : uint8_t {
	DEFAULT
};

Microsoft::WRL::ComPtr<ID3D11DepthStencilView> MakeDefaultDSS();

