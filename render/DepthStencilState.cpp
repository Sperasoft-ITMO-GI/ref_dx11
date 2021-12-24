#include "DepthStencilState.h"

using Microsoft::WRL::ComPtr;

ComPtr<ID3D11DepthStencilView> MakeDefaultDSS() {
	Renderer* renderer = Renderer::GetInstance();
	auto parameters = renderer->GetWindowParameters();

	D3D11_TEXTURE2D_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_TEXTURE2D_DESC));
	depth_stencil_desc.Width = std::get<0>(parameters);
	depth_stencil_desc.Height = std::get<1>(parameters);
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.SampleDesc.Count = renderer->IsMsaaEnable() ? 4 : 1;
	depth_stencil_desc.SampleDesc.Quality = renderer->IsMsaaEnable() ? (renderer->GetMSAAQuality() - 1) : 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;

	ID3D11Texture2D* buffer;
	ComPtr<ID3D11DepthStencilView> view;

	DXCHECK(renderer->GetDevice()->CreateTexture2D(&depth_stencil_desc, 0, &buffer));
	DXCHECK(renderer->GetDevice()->CreateDepthStencilView(buffer, 0, &view));

	//renderer->GetContext()->OMSetRenderTargets(1, &renderer->GetRenderTargetView(), NULL);

	return view;
}