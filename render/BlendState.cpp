#include "BlendState.h"

using Microsoft::WRL::ComPtr;

ComPtr<ID3D11BlendState> MakeAlphaBS() {
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BLEND_DESC state_desc;
	ZeroMemory(&state_desc, sizeof(D3D11_BLEND_DESC));
	state_desc.AlphaToCoverageEnable = false;
	state_desc.IndependentBlendEnable = false;

	// with alpha [0]

	state_desc.RenderTarget[0].BlendEnable = true;
	state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// without alpha [1]

	state_desc.RenderTarget[1].BlendEnable = true;
	state_desc.RenderTarget[1].SrcBlend = D3D11_BLEND_SRC_COLOR;
	state_desc.RenderTarget[1].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	state_desc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
	state_desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
	state_desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ComPtr<ID3D11BlendState> state;
	renderer->GetDevice()->CreateBlendState(&state_desc, &state);

	return state;
};