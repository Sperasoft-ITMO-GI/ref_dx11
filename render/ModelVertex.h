#pragma once

#include <WndDxIncludes.h>

struct ModelVertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture_coord;
	DirectX::XMFLOAT2 lightmap_coord;
};