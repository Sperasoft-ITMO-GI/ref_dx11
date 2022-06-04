#pragma once 

#include <vector>
#include <array>
#include <unordered_map>
#include <set>
#include <algorithm>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <BSPPoly.h>

// There should be defines
#define BSP_ALPHA				0x02
#define BSP_WATER				0x04
#define BSP_LIGHTMAPPEDPOLY		0x08
#define BSP_TEX                 0x10
#define BSP_TEX_WIREFRAME		0x20
#define BSP_TEX_LIGHTMAP        0x40
#define BSP_TEX_LIGHTMAPWIRE    0x80

struct BSPDefinitions {
	std::vector<BSPVertex> vert;
	std::vector<UINT> ind;
	MODEL cb;
	MatrixBuffer mb;
	int flags;
	int texture_index;
	int lightmap_index;
};

class BSPRenderer {
public:

	~BSPRenderer();

	void Init();

	void InitCB();

	void Render();

	void AddElement(const BSPPoly& polygon);

	void Add(const BSPDefinitions& polygon);
	void Add2d(const BSPDefinitions& polygon);
	void Add2d2(const BSPDefinitions& polygon);

	void InitNewFactory(const wchar_t* path);

	bool CompileWithDefines(int defines);

	void BindNewFactory();

	void ClearTempFactory();

	void ClearLightMaps();

private:
	class ModelPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
		bool is_first_lm_pass = true;
	};

	bool IsGarbagePolygon(int texture_index);
	
private:
	PipelineFactory* factory;
	PipelineFactory* factory_temp;
	std::vector<BSPPoly> polygons;
	BSPPoly* p;
	ConstantBuffer<MatrixBuffer> mat_buf;

	BSPDefinitions level;
	std::vector<BSPDefinitions> bsp_defs;
	std::vector<BSPDefinitions> bsp_defs2d;

	int currentState;

	ID3D11Texture2D* pos_texs[32];
	ID3D11Texture2D* norm_texs[32];
	ID3D11Texture2D* alb_texs[32];
	ID3D11Texture2D* lightmap_texs[32];
	ID3D11RenderTargetView* pos_rtv[32];
	ID3D11RenderTargetView* norm_rtv[32];
	ID3D11RenderTargetView* alb_rtv[32];
	ID3D11RenderTargetView* lightmap_rtv[32];
	ID3D11ShaderResourceView* pos_srv[32];
	ID3D11ShaderResourceView* norm_srv[32];
	ID3D11ShaderResourceView* alb_srv[32];
	ID3D11ShaderResourceView* lightmap_srv[32];

	D3D11_VIEWPORT viewports[2];
	

public:
	ID3D11SamplerState* samplers[2];
	ModelPSProvider* GetPSProvider();

	bool is_first = true;
	bool is_lightmap = false;
	bool is_gbuffer = false;
	bool is_lighting = false;
	int lightmap_index = 0;

	std::array<DirectX::XMFLOAT3, 1000> light_sources;
	int light_source_index = 0;
	DirectX::XMFLOAT4 point_light_buf;

	ConstantBuffer<LightSources> light_sources_buffer;
};