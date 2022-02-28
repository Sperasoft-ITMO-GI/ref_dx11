#pragma once

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <ParticlesPoly.h>

// There should be defines
#define PARTICLES_DEFAULT    0x001

struct ParticlesDefinitions {
	std::vector<ParticlesVertex> vert;
	std::vector<uint16_t> ind;
	MODEL cbp;
	int flags;
	int texture_index;
};

class ParticlesRenderer {
public:

	~ParticlesRenderer();

	void Init();

	void InitCB();

	void Render();

	void Add(const ParticlesDefinitions& polygon);

	void InitNewFactory(const wchar_t* path);

	bool CompileWithDefines(int defines);

	void BindNewFactory();

	void ClearTempFactory();

private:
	class ParticlesPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	PipelineFactory* factory_temp;
	ParticlesPoly* p;
	std::vector<ParticlesDefinitions> particles_defs;

	int currentState;
};