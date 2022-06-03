#pragma once 

#include <vector>
#include <unordered_map>
#include <algorithm>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <UtilsPoly.h>
#include <EffectsQuad.h>

// There should be defines
#define UTILS_STATIC  0x001
#define UTILS_DYNAMIC 0x002
#define UTILS_QUAD    0x004


struct UtilsDefinitions {
	std::vector<UtilsVertex> vert;
	std::vector<UINT> ind;
	MODEL cb;
	int flags;
};

class UtilsRenderer {
public:

	~UtilsRenderer();

	void Init();

	void InitCB();

	void Render();

	void RenderQuad();


	void Add(const UtilsDefinitions& polygon);

	void InitNewFactory(const wchar_t* path);

	bool CompileWithDefines(int defines);

	void BindNewFactory();

	void ClearTempFactory();

private:
	class UtilsPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	PipelineFactory* factory_temp;
	UtilsPoly* p;
	EffectsQuad* quad;
	std::vector<UtilsDefinitions> Utils_defs;
	std::vector<UtilsDefinitions> temp;
	ConstantBuffer<MatrixBuffer> cbuffer;
	int currentState;
};