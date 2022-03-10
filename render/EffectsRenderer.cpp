#include <EffectsRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static D3D_SHADER_MACRO sceneMac[] = {
	"SCENE", "1", NULL, NULL
};

static D3D_SHADER_MACRO bloomMac[] = {
	"BLOOM", "1", NULL, NULL
};

static ShaderOptions defOpt{
	defMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions sceneOpt{
	sceneMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions bloomOpt{
	bloomMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{EFFECTS_DEFAULT, defOpt},
	{EFFECTS_SCENE, sceneOpt},
	{EFFECTS_BLOOM, bloomOpt},
};

EffectsRenderer::~EffectsRenderer() {
	delete factory;
	delete eq;
}

void EffectsRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Effects.hlsl", new EffectsPSProvider(), macro);
	eq = new EffectsQuad();
}

void EffectsRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	if (is_render) {
		SetPipelineState(factory->GetState(EFFECTS_DEFAULT));
		renderer->GetContext()->OMSetRenderTargets(
			1u, 
			&renderer->render_target_views[EffectsRTV::EFFECT], 
			nullptr
		);
		eq->DrawStatic();
	}

	SetPipelineState(factory->GetState(EFFECTS_BLOOM));
	renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM], nullptr);
	renderer->GetContext()->PSSetShaderResources(sceneTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::SCENE_SRV]);
	eq->DrawStatic();

	SetPipelineState(factory->GetState(EFFECTS_SCENE));
	renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BACKBUFFER], renderer->GetDepthStencilView());
	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_SRV]);
	renderer->GetContext()->PSSetShaderResources(effectTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::EFFECT_SRV]);
	eq->DrawStatic();

	ID3D11ShaderResourceView* null_SRV[1] = { nullptr };
	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot,  1u, null_SRV);
	renderer->GetContext()->PSSetShaderResources(sceneTexture.slot,  1u, null_SRV);
	renderer->GetContext()->PSSetShaderResources(effectTexture.slot, 1u, null_SRV);
}

void EffectsRenderer::InitCB()
{
	MODEL cb;
	eq->CreateCB(cb);
}

void EffectsRenderer::Add(MODEL& cbe)
{
	eq->UpdateCB(cbe);
}

void EffectsRenderer::EffectsPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	state->bs = states->blend_states.at(BlendState::ALPHABS);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
	state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
	state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::EFFECTS_QUAD));
	state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
}


void EffectsRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new EffectsPSProvider(), macro);
}

bool EffectsRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void EffectsRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;

	factory_temp = nullptr;
}

void EffectsRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}
