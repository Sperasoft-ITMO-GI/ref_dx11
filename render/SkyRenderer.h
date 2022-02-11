#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <SkyPoly.h>
#include <Utils.hpp>

#define SKY_DEFAULT  0x001

class SkyRenderer {
public:

	~SkyRenderer();

	void Init();

	void Render();

	void InitCB();

	//void Add(const SkyDefinitions& quad);

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
	SkyPoly* sp;

public:
	bool is_exist = false;
};