#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <Quad.h>
#include <Utils.hpp>

#define SKY_DEFAULT  0x001

struct SkyDefinitions {
	std::vector<SkyVertex> vert;
	std::vector<uint16_t> ind;
	ConstantBufferQuad cbq;
	int flags;
	int texture_index;
};

class SkyRenderer {
public:

	~SkyRenderer();

	void Init();

	void Render();

	void InitCB();

	void AddElement(const Quad& quad); // должен принимать Quad или что то типо такого

	void Add(const SkyDefinitions& quad);

	void InitNewFactory(const wchar_t* path);

	void CompileWithDefines(int defines);

	void BindNewFactory();

	void ClearTempFactory();

private:
	class SkyPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	PipelineFactory* factory_temp;
	std::vector<Quad> quads;
	Quad* q;
	std::vector<SkyDefinitions> sky_defs;
};