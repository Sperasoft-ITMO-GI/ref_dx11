#pragma once

#include <WndDxIncludes.h>

struct BSPVertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture_coord;
	DirectX::XMFLOAT2 lightmap_texture_coord;
};