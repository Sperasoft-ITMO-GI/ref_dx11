#pragma once

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <ModPoly.h>

// There should be defines
#define MOD_ALPHA				0x001

struct ModDefinitions {
	std::vector<ModVertex> vert;
	std::vector<uint16_t> ind;
	ConstantBufferPolygon cbp;
	int flags;
	int texture_index;
	int lightmap_index;
};

class ModRenderer {
public:

	~ModRenderer();

	void Init();

	void InitCB();

	void Render();

	void AddElement(const ModPoly& polygon);

	void Add(const ModDefinitions& polygon);

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
	std::vector<ModPoly> polygons;
	ModPoly* p;
	std::vector<ModDefinitions> mod_defs;
};