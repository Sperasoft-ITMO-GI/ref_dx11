#pragma once 

#include <vector>
#include <unordered_map>

#include <WndDxIncludes.h>
#include <PipelineFactory.h>
#include <Renderer.h>
#include <IStateProvider.h>
#include <EffectsQuad.h>

#define EFFECTS_DEFAULT  0x001
#define EFFECTS_SCENE    0x002

class EffectsRenderer {
public:

	~EffectsRenderer();

	void Init();

	void Render();

	void InitCB();

	void Add(ConstantBufferEffects& cbe);

	void InitNewFactory(const wchar_t* path);

	void CompileWithDefines(int defines);

	void BindNewFactory();

	void ClearTempFactory();

private:
	class EffectsPSProvider : public IStateProvider {
	public:
		virtual void PatchPipelineState(PipelineState* state, int defines) override;
	};

private:
	PipelineFactory* factory;
	PipelineFactory* factory_temp;
	EffectsQuad* eq;

	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* render_target;
	ID3D11ShaderResourceView* resource_view;

public:
	bool is_render = false;
};