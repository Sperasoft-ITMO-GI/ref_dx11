#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <BeamPoly.h>

// There should be defines
#define BEAM_DEFAULT 0x001


struct BeamDefinitions {
	std::vector<BeamVertex> vert;
	std::vector<uint16_t> ind;
	MODEL cb;
	int flags;
};

class BeamRenderer {
public:

	~BeamRenderer();

	void Init();

	void InitCB();

	void Render();

	void Add(const BeamDefinitions& polygon);

	void InitNewFactory(const wchar_t* path);

	bool CompileWithDefines(int defines);

	void BindNewFactory();

	void ClearTempFactory();

private:
	class BeamPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	PipelineFactory* factory_temp;
	BeamPoly* p;
	std::vector<BeamDefinitions> beam_defs;

	int currentState;
};