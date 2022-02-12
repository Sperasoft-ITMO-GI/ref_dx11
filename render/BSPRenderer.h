#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <BSPPoly.h>
#include <Utils.hpp>

// There should be defines
#define BSP_SOLID				0x001
#define BSP_ALPHA				0x002
#define BSP_WATER				0x004
#define BSP_LIGHTMAP			0x008
#define BSP_LIGHTMAPPEDPOLY		0x010

struct BSPDefinitions {
	std::vector<BSPVertex> vert;
	std::vector<uint16_t> ind;
	ConstantBufferPolygon cbp;
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

	void InitNewFactory(const wchar_t* path);

	void CompileWithDefines(int defines);

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
	std::vector<BSPDefinitions> bsp_defs;
};