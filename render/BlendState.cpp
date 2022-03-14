#include <BlendState.h>

ID3D11BlendState* MakeUIAlphaBS() {
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BLEND_DESC state_desc;
	ZeroMemory(&state_desc, sizeof(D3D11_BLEND_DESC));
	state_desc.AlphaToCoverageEnable = false;
	state_desc.IndependentBlendEnable = false;

	state_desc.RenderTarget[0].BlendEnable = true;
	state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* state;
	renderer->GetDevice()->CreateBlendState(&state_desc, &state);

	return state;
};

ID3D11BlendState* MakeNoBS() {
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BLEND_DESC state_desc;
	ZeroMemory(&state_desc, sizeof(D3D11_BLEND_DESC));
	state_desc.AlphaToCoverageEnable = false;
	state_desc.IndependentBlendEnable = false;

	state_desc.RenderTarget[0].BlendEnable = false;
	state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ID3D11BlendState* state;
	renderer->GetDevice()->CreateBlendState(&state_desc, &state);

	return state;
}

ID3D11BlendState* MakeAlphaBS()
{
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BLEND_DESC state_desc;
	ZeroMemory(&state_desc, sizeof(D3D11_BLEND_DESC));
	state_desc.AlphaToCoverageEnable = false;
	state_desc.IndependentBlendEnable = false;

	state_desc.RenderTarget[0].BlendEnable = true;
	state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* state;
	renderer->GetDevice()->CreateBlendState(&state_desc, &state);

	return state;
}

ID3D11BlendState* MakeSurfLightMapBS()
{
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BLEND_DESC state_desc;
	ZeroMemory(&state_desc, sizeof(D3D11_BLEND_DESC));
	state_desc.AlphaToCoverageEnable = false;
	state_desc.IndependentBlendEnable = false;

	state_desc.RenderTarget[0].BlendEnable = true;
	state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
	state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
	state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* state;
	renderer->GetDevice()->CreateBlendState(&state_desc, &state);

	return state;
}

ID3D11BlendState* MakeEffectBS()
{
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BLEND_DESC state_desc;
	ZeroMemory(&state_desc, sizeof(D3D11_BLEND_DESC));
	state_desc.AlphaToCoverageEnable = false;
	state_desc.IndependentBlendEnable = false;

	state_desc.RenderTarget[0].BlendEnable = true;
	state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* state;
	renderer->GetDevice()->CreateBlendState(&state_desc, &state);

	return state;
}

ID3D11BlendState* MakeScreenBS()
{
	Renderer* renderer = Renderer::GetInstance();

	D3D11_BLEND_DESC state_desc;
	ZeroMemory(&state_desc, sizeof(D3D11_BLEND_DESC));
	state_desc.AlphaToCoverageEnable = false;
	state_desc.IndependentBlendEnable = false;

	state_desc.RenderTarget[0].BlendEnable = true;
	state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* state;
	renderer->GetDevice()->CreateBlendState(&state_desc, &state);

	return state;
}
