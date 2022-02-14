#pragma once

#include <WndDxIncludes.h>

struct ModVertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 texture_coord;
	DirectX::XMFLOAT2 lightmap_texture_coord;
};