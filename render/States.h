#pragma once

#include <unordered_map>

#include <WndDxIncludes.h>
#include <BlendState.h>
#include <RasterizationState.h>
#include <DepthStencilState.h>
#include <InputLayout.h>
#include <Topology.h>

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

	const std::unordered_map<BlendState, ID3D11BlendState*> blend_states{
		{UIALPHABS,   MakeUIAlphaBS()},
		{ALPHABS, MakeAlphaBS()},
		{SURFLIGHTMAPBS, MakeSurfLightMapBS()},
		{NOBS, MakeNoBS()},
		{EFFECTBS, MakeEffectBS()},
		{SCREEN, MakeScreenBS()}
	};
	const std::unordered_map<RasterizationState, ID3D11RasterizerState*> rasterization_states{
		{CULL_NONE,  MakeCullNoneRS()},
		{CULL_BACK,  MakeCullBackRS()},
		{CULL_FRONT, MakeCullFrontRS()},
		{WIREFRAME, MakeWireframeRS()}
	};
	const std::unordered_map<DepthStencilState, ID3D11DepthStencilState*> depth_stencil_states{
		{NEVER,      MakeNeverDSS()},
		{LESS,       MakeLessDSS()},
		{LESS_EQUAL, MakeLessEqualDSS()},
	};
	const std::unordered_map<Layout, std::vector<D3D11_INPUT_ELEMENT_DESC>> input_layouts{		
		{
			UI_QUAD, 
			{
				{"Position", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
		{
			SKY_POLYGON,
			{
				{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			}
		},
		{
			BSP_POLYGON,
			{
				{"Position",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TexCoord",	0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"LmTexCoord",  0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
		{
			BEAM_POLYGON,
			{
				{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
		{
			MOD_POLYGON,
			{
				{"Position",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"Color_In",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TexCoord",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"LmTexCoord",  0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},		
        {
			PARTICLES_POLYGON,
			{
				{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"Color",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
		{
			EFFECTS_QUAD,
			{
				{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
	};
	const std::unordered_map<Topology, D3D_PRIMITIVE_TOPOLOGY> topology{
		{POINT_LISTS,    D3D11_PRIMITIVE_TOPOLOGY_POINTLIST     },
		{LINE_LISTS,     D3D11_PRIMITIVE_TOPOLOGY_LINELIST      },
		{TRIANGLE_LISTS, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST  },
		{TRIANGLE_STRIP, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP	},
		{LINE_STRIP,	 D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP		}
	};

private:
	States() = default;
	static States* states;
};