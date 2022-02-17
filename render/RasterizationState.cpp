#include <RasterizationState.h>

ID3D11RasterizerState* MakeCullNoneRS() {
	Renderer* renderer = Renderer::GetInstance();

	D3D11_RASTERIZER_DESC rasterizer_description;
	ZeroMemory(&rasterizer_description, sizeof(D3D11_RASTERIZER_DESC));

	rasterizer_description.AntialiasedLineEnable = FALSE;
	rasterizer_description.CullMode = D3D11_CULL_NONE;
	rasterizer_description.DepthBias = 0;
	rasterizer_description.DepthBiasClamp = 0.0f;
	rasterizer_description.DepthClipEnable = TRUE;
	rasterizer_description.FillMode = D3D11_FILL_SOLID;
	rasterizer_description.FrontCounterClockwise = FALSE;
	rasterizer_description.MultisampleEnable = FALSE;
	rasterizer_description.ScissorEnable = FALSE;
	rasterizer_description.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* state;

	renderer->GetDevice()->CreateRasterizerState(&rasterizer_description, &state);

	return state;
}

ID3D11RasterizerState* MakeCullBackRS() {
	Renderer* renderer = Renderer::GetInstance();

	D3D11_RASTERIZER_DESC rasterizer_description;
	ZeroMemory(&rasterizer_description, sizeof(D3D11_RASTERIZER_DESC));

	rasterizer_description.AntialiasedLineEnable = FALSE;
	rasterizer_description.CullMode = D3D11_CULL_BACK;
	rasterizer_description.DepthBias = 0;
	rasterizer_description.DepthBiasClamp = 0.0f;
	rasterizer_description.DepthClipEnable = TRUE;
	rasterizer_description.FillMode = D3D11_FILL_SOLID;
	rasterizer_description.FrontCounterClockwise = FALSE;
	rasterizer_description.MultisampleEnable = FALSE;
	rasterizer_description.ScissorEnable = FALSE;
	rasterizer_description.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* state;

	renderer->GetDevice()->CreateRasterizerState(&rasterizer_description, &state);

	return state;
}

ID3D11RasterizerState* MakeCullFrontRS() {
	Renderer* renderer = Renderer::GetInstance();

	D3D11_RASTERIZER_DESC rasterizer_description;
	ZeroMemory(&rasterizer_description, sizeof(D3D11_RASTERIZER_DESC));

	rasterizer_description.AntialiasedLineEnable = FALSE;
	rasterizer_description.CullMode = D3D11_CULL_FRONT;
	rasterizer_description.DepthBias = 0;
	rasterizer_description.DepthBiasClamp = 0.0f;
	rasterizer_description.DepthClipEnable = TRUE;
	rasterizer_description.FillMode = D3D11_FILL_SOLID;
	rasterizer_description.FrontCounterClockwise = FALSE;
	rasterizer_description.MultisampleEnable = FALSE;
	rasterizer_description.ScissorEnable = FALSE;
	rasterizer_description.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* state;

	renderer->GetDevice()->CreateRasterizerState(&rasterizer_description, &state);

	return state;
}

ID3D11RasterizerState* MakeWireframeRS()
{
	Renderer* renderer = Renderer::GetInstance();

	D3D11_RASTERIZER_DESC rasterizer_description;
	ZeroMemory(&rasterizer_description, sizeof(D3D11_RASTERIZER_DESC));

	rasterizer_description.AntialiasedLineEnable = FALSE;
	rasterizer_description.CullMode = D3D11_CULL_NONE;
	rasterizer_description.DepthBias = 0;
	rasterizer_description.DepthBiasClamp = 0.0f;
	rasterizer_description.DepthClipEnable = TRUE;
	rasterizer_description.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_description.FrontCounterClockwise = FALSE;
	rasterizer_description.MultisampleEnable = FALSE;
	rasterizer_description.ScissorEnable = FALSE;
	rasterizer_description.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState* state;

	renderer->GetDevice()->CreateRasterizerState(&rasterizer_description, &state);

	return state;
}