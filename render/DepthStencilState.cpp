#include <DepthStencilState.h>

ID3D11DepthStencilState* MakeNeverDSS() {
	Renderer* renderer = Renderer::GetInstance();
	ID3D11DepthStencilState* depth_stencil_state;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depth_stencil_desc.DepthEnable = false;
	DXCHECK(renderer->GetDevice()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state));

	return depth_stencil_state;
}

ID3D11DepthStencilState* MakeLessDSS() {
	Renderer* renderer = Renderer::GetInstance();
	ID3D11DepthStencilState* depth_stencil_state;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	DXCHECK(renderer->GetDevice()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state));

	return depth_stencil_state;
}

ID3D11DepthStencilState* MakeLessEqualDSS() {
	Renderer* renderer = Renderer::GetInstance();
	ID3D11DepthStencilState* depth_stencil_state;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DXCHECK(renderer->GetDevice()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state));

	return depth_stencil_state;
}