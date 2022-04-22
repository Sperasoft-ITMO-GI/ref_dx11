#pragma once 

#include <vector>
#include <unordered_map>
#include <algorithm>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <BSPPoly.h>

// There should be defines
#define BSP_ALPHA				0x002
#define BSP_WATER				0x004
#define BSP_LIGHTMAPPEDPOLY		0x008
#define BSP_TEX                 0x016
#define BSP_LMTEX               0x032

struct BSPDefinitions {
	std::vector<BSPVertex> vert;
	std::vector<uint16_t> ind;
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

private:
	class ModelPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	PipelineFactory* factory_temp;
	std::vector<BSPPoly> polygons;
	BSPPoly* p;
	ConstantBuffer<MatrixBuffer> mat_buf;
	std::vector<BSPDefinitions> bsp_defs;
	std::vector<BSPDefinitions> bsp_defs2d;
	std::vector<BSPDefinitions> bsp_defs2d2;

	int currentState;

	ID3D11Texture2D* texs[3];
	ID3D11RenderTargetView* views[3];
	ID3D11ShaderResourceView* srv[3];

public:
	bool is_gbuffer = false;
};