#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <EffectsQuad.h>

#define EFFECTS_DEFAULT          0x001
#define EFFECTS_SCENE            0x002
#define EFFECTS_GLOW             0x004
#define EFFECTS_HORIZONTAL_BLUR  0x008
#define EFFECTS_VERTICAL_BLUR    0x016
#define EFFECTS_INTENSITY        0x032
#define EFFECTS_FXAA             0x064
#define EFFECTS_MOTION_BLUR      0x128
#define EFFECTS_TAA              0x256

class EffectsRenderer {
public:

	~EffectsRenderer();

	void Init();

	void Render();

	void InitCB();

	void Add(MODEL& cbe);

	void InitNewFactory(const wchar_t* path);

	bool CompileWithDefines(int defines);

	void BindNewFactory();

	void ClearTempFactory();

	void UnBindRenderTargets();
	void UnBindShaderResourceViews();
	void ClearRenderTargetViews();

private:
	class EffectsPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	PipelineFactory* factory_temp;
	EffectsQuad* eq;

	//ID3D11Texture2D* texture;
	//ID3D11RenderTargetView* render_target;
	//ID3D11ShaderResourceView* resource_view;

	int currentState;
public:
	bool is_render = false;
	bool fxaa = false;
	bool is_first = true;
};