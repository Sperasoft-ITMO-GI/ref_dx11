#pragma once

#include <vector>

#include <WndDxIncludes.h>
#include <Renderer.h>

enum Layout {
	COLOR,
	TEXTURE
};

Microsoft::WRL::ComPtr<ID3D11InputLayout> MakeLayout(Microsoft::WRL::ComPtr<ID3DBlob> blob, std::vector<D3D11_INPUT_ELEMENT_DESC> desc);
