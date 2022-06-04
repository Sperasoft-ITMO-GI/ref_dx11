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
#define EFFECTS_VERTICAL_BLUR    0x010
#define EFFECTS_INTENSITY        0x020
#define EFFECTS_FXAA             0x040
#define EFFECTS_MOTION_BLUR      0x080
#define EFFECTS_TAA              0x100
#define EFFECTS_LIGHT            0x200

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

	void ChangeFlags(float pos, float norm, float alb);

	void RenderLight(Renderer* renderer);

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

	void RenderColorPostEffects(Renderer* renderer);
	void RenderMotionBlur(Renderer* renderer);
	void RenderBloom(Renderer* renderer);
	void RenderTAA(Renderer* renderer);
	void RenderFXAA(Renderer* renderer);
	void RenderToBackbuffer(Renderer* renderer);
public:
	bool is_render = false;
	bool fxaa = false;
	bool is_first = true;

	bool positions = false;
	bool normals = false;
	bool albedo = false;
};