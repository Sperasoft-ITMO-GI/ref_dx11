#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <SkyCube.h>

#define SKY_DEFAULT  0x001

class SkyRenderer {
public:

	~SkyRenderer();

	void Init();

	void Render();

	void InitCB();

	void Add(ConstantBufferSkyBox& cbsb);

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
	SkyCube* sp;

	int sky_box_order[6] = { 3, 1, 4, 5, 0, 2 };

public:
	bool is_exist = false;
};