#pragma once

#include <unordered_map>

#include <WndDxIncludes.h>
#include <DepthStencilState.h>
#include <BlendState.h>
#include <RasterizationState.h>
#include <InputLayout.h>

class States {
public:
	States(const States&) = delete;
	States(States&&) = delete;
	States& operator=(const States&) = delete;

	static States* GetInstance() {
		if (states == nullptr) {
			states = new States();
		}
		return states;
	}

	const std::unordered_map<DepthStencilState, Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> dssStates{
		{DEFAULT, MakeDefaultDSS()}
	};
	const std::unordered_map<RasterizationState, Microsoft::WRL::ComPtr<ID3D11RasterizerState>> rasterization_states{
		{CULL_NONE, MakeCullNoneRS()}
	};
	const std::unordered_map<BlendState, Microsoft::WRL::ComPtr<ID3D11BlendState>> blend_states{
		{ALPHA, MakeAlphaBS()}
	};
	const std::unordered_map<Layout, std::vector<D3D11_INPUT_ELEMENT_DESC>> input_layouts{
		{
			COLOR, 
			{
				{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"Color", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
		{
			TEXTURE, 
			{
				{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"Color", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}

		}
	};

private:
	States() = default;
	static States* states;
};