#include <EffectsRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static D3D_SHADER_MACRO sceneMac[] = {
	"SCENE", "1", NULL, NULL
};

static D3D_SHADER_MACRO glowMac[] = {
	"GLOW", "1", NULL, NULL
};

static D3D_SHADER_MACRO horizontalBlurMac[] = {
	"HORIZONTAL_BLUR", "1", NULL, NULL
};

static D3D_SHADER_MACRO verticalBlurMac[] = {
	"VERTICAL_BLUR", "1", NULL, NULL
};

static D3D_SHADER_MACRO intensityMac[] = {
	"INTENSITY", "1", NULL, NULL
};

static D3D_SHADER_MACRO fxaaMac[] = {
	"FXAA", "1", NULL, NULL
};

static D3D_SHADER_MACRO mbMac[] = {
	"MOTION_BLUR", "1", NULL, NULL
};

static ShaderOptions defOpt{
	defMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions sceneOpt{
	sceneMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions glowOpt{
	glowMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions horizontalBlurOpt{
	horizontalBlurMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions verticalBlurOpt{
	verticalBlurMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions intensityOpt{
	intensityMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions fxaaOpt{
	fxaaMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions mbOpt{
	mbMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{EFFECTS_DEFAULT, defOpt},
	{EFFECTS_SCENE, sceneOpt},
	{EFFECTS_GLOW, glowOpt},
	{EFFECTS_HORIZONTAL_BLUR, horizontalBlurOpt},
	{EFFECTS_VERTICAL_BLUR, verticalBlurOpt},
	{EFFECTS_INTENSITY, intensityOpt},
	{EFFECTS_FXAA, fxaaOpt},
	{EFFECTS_MOTION_BLUR, mbOpt},
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
		BEGIN_EVENT(L"Default effect")
		SetPipelineState(factory->GetState(EFFECTS_DEFAULT));
		renderer->GetContext()->OMSetRenderTargets(
			1u, 
			&renderer->render_target_views[EffectsRTV::EFFECT], 
			nullptr
		);
		eq->DrawStatic();
		END_EVENT();
	}
	ID3D11ShaderResourceView* null_SRV[1] = { nullptr };
	renderer->GetContext()->ClearRenderTargetView(renderer->render_target_views[EffectsRTV::BLOOM_1], DirectX::Colors::Black);
	renderer->GetContext()->ClearRenderTargetView(renderer->render_target_views[EffectsRTV::BLOOM_2], DirectX::Colors::Black);

	BEGIN_EVENT(L"Motion blur pass")
	SetPipelineState(factory->GetState(EFFECTS_MOTION_BLUR));
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, null_SRV);
	renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM_1], renderer->GetDepthStencilView());
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::SCENE_SRV]);
	renderer->GetContext()->PSSetShaderResources(depthTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::DEPTH_SRV]);
	eq->DrawStatic();
	END_EVENT();

	BEGIN_EVENT(L"Bloom pass")
	BEGIN_EVENT(L"Bloom Threshold")
	SetPipelineState(factory->GetState(EFFECTS_GLOW));
	renderer->GetContext()->OMSetRenderTargets(0u, nullptr, nullptr);
	renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM_2], renderer->GetDepthStencilView());
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_1_SRV]);
	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::MASK_SRV]);
	eq->DrawStatic();
	END_EVENT();
	
	renderer->GetContext()->ClearRenderTargetView(renderer->render_target_views[EffectsRTV::SCENE], DirectX::Colors::Black);
	BEGIN_EVENT(L"Blur pass")
	int kernel = 4;
	for (int i = 0; i < kernel; ++i) {

		SetPipelineState(factory->GetState(EFFECTS_HORIZONTAL_BLUR));
		renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::SCENE], renderer->GetDepthStencilView());
		renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_2_SRV]);
		eq->DrawStatic();

		renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, null_SRV);

		SetPipelineState(factory->GetState(EFFECTS_VERTICAL_BLUR));
		renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM_2], renderer->GetDepthStencilView());
		renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::SCENE_SRV]);
		eq->DrawStatic();

		renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, null_SRV);
	}
	END_EVENT();

	BEGIN_EVENT(L"Bloom intensity")
	SetPipelineState(factory->GetState(EFFECTS_INTENSITY));
	renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM_2], renderer->GetDepthStencilView());
	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::SCENE_SRV]);
	eq->DrawStatic();
	END_EVENT();
	END_EVENT();

	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, null_SRV);

	BEGIN_EVENT(L"FXAA pass")
	//renderer->GetContext()->ClearRenderTargetView(renderer->render_target_views[EffectsRTV::BLOOM_1], DirectX::Colors::Black);
	SetPipelineState(factory->GetState(EFFECTS_FXAA));
	renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::FXAA], renderer->GetDepthStencilView());
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_1_SRV]);
	eq->DrawStatic();
	
	SetPipelineState(factory->GetState(EFFECTS_SCENE));
	renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BACKBUFFER], renderer->GetDepthStencilView());
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::FXAA_SRV]);
	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_2_SRV]);
	renderer->GetContext()->PSSetShaderResources(effectTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::EFFECT_SRV]);
	eq->DrawStatic();
	END_EVENT();
	//ID3D11ShaderResourceView* null_SRV[1] = { nullptr };
	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot,  1u, null_SRV);
	renderer->GetContext()->PSSetShaderResources(fxaaTexture.slot,  1u, null_SRV);
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot,  1u, null_SRV);
	renderer->GetContext()->PSSetShaderResources(lightmapTexture.slot,  1u, null_SRV);
	renderer->GetContext()->PSSetShaderResources(effectTexture.slot, 1u, null_SRV);
	renderer->GetContext()->PSSetShaderResources(depthTexture.slot, 1u, null_SRV);
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
	Renderer* renderer = Renderer::GetInstance();
	if (defines & EFFECTS_GLOW) {
		state->bs = states->blend_states.at(BlendState::NOBS);

		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	}
	if (defines & EFFECTS_HORIZONTAL_BLUR) {
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	}
	if (defines & EFFECTS_VERTICAL_BLUR) {
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	}
	if (defines & EFFECTS_SCENE) {
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	}
	if (defines & EFFECTS_DEFAULT) {
		state->bs = states->blend_states.at(BlendState::ALPHABS);

	}
	if (defines & EFFECTS_FXAA) {
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	}	
	if (defines & EFFECTS_MOTION_BLUR) {
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	}

	//state->dss = states->depth_stencil_states.at(DepthStencilState::NEVER);
	//state->bs = states->blend_states.at(BlendState::NOBS);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
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
