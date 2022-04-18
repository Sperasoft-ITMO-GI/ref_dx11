#pragma once 

#include <vector>
#include <unordered_map>
#include <algorithm>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <UtilsPoly.h>

// There should be defines
#define UTILS_STATIC  0x001
#define UTILS_DYNAMIC 0x002


struct UtilsDefinitions {
	std::vector<UtilsVertex> vert;
	std::vector<uint16_t> ind;
	MODEL cb;
	int flags;
};

class UtilsRenderer {
public:

	~UtilsRenderer();

	void Init();

	void InitCB();

	void Render();

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
	std::vector<UtilsDefinitions> Utils_defs;
	std::vector<UtilsDefinitions> temp;

	int currentState;
};