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

static D3D_SHADER_MACRO taaMac[] = {
	"TAA", "1", NULL, NULL
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

static ShaderOptions taaOpt{
	taaMac,
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
	{EFFECTS_TAA, taaOpt},
};

EffectsRenderer::~EffectsRenderer() {
	delete factory;
	delete eq;
}

void EffectsRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Effects.hlsl", new EffectsPSProvider(), macro);
	eq = new EffectsQuad();
}

void EffectsRenderer::UnBindShaderResourceViews() {
	Renderer* renderer = Renderer::GetInstance();
	ID3D11ShaderResourceView* null_srv[1] = { nullptr };
	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot,        1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(fxaaTexture.slot,         1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot,        1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(lightmapTexture.slot,     1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(effectTexture.slot,       1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(depthTexture.slot,        1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(prevdepthTexture.slot,    1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(velocityTexture.slot,     1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(prevvelocityTexture.slot, 1u, null_srv);
	renderer->GetContext()->PSSetShaderResources(prevcolorTexture.slot,    1u, null_srv);
}

void EffectsRenderer::ChangeFlags(float pos, float norm, float alb)
{
	// пока пусть так, но тут закрался баг
	if (positions != static_cast<bool>(pos)) {
		positions = pos;
		normals = false;
		albedo = false;
	}
	else if (normals != static_cast<bool>(norm)) {
		normals = norm;
		positions = false;
		albedo = false;
	}
	else if (albedo != static_cast<bool>(alb)){
		albedo = alb;
		positions = false;
		normals = false;
	}
}

void EffectsRenderer::UnBindRenderTargets() {
	Renderer* renderer = Renderer::GetInstance();
	ID3D11RenderTargetView* render_targets[] = {
		renderer->render_target_views[SCENE],
		renderer->render_target_views[SCENE_HIST],
		renderer->render_target_views[LIGHTMAP],
		renderer->render_target_views[MASK],
		renderer->render_target_views[MOTION_BLUR],
		renderer->render_target_views[BLOOM_1],
		NULL
	};
	renderer->GetContext()->OMSetRenderTargets(7, render_targets, nullptr);

	render_targets[0] = renderer->render_target_views[BLOOM_2];
	render_targets[1] = renderer->render_target_views[FXAA];
	render_targets[2] = renderer->render_target_views[EFFECT];
	render_targets[3] = renderer->render_target_views[VELOSITY];
	render_targets[4] = renderer->render_target_views[VELOSITY_HIST];
	render_targets[5] = renderer->render_target_views[SCENE_COLOR];
	render_targets[6] = NULL;
	renderer->GetContext()->OMSetRenderTargets(7, render_targets, nullptr);
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

	if (!positions && !normals && !albedo) {
		ID3D11ShaderResourceView* null_SRV[1] = { nullptr };
		renderer->GetContext()->ClearRenderTargetView(renderer->render_target_views[EffectsRTV::BLOOM_1], DirectX::Colors::Black);
		renderer->GetContext()->ClearRenderTargetView(renderer->render_target_views[EffectsRTV::BLOOM_2], DirectX::Colors::Black);
		UnBindRenderTargets();
		UnBindShaderResourceViews();

		BEGIN_EVENT(L"Motion blur pass")
			SetPipelineState(factory->GetState(EFFECTS_MOTION_BLUR));
		renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, null_SRV);
		if (renderer->index == 0)
			renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::MOTION_BLUR], nullptr);
		else
			renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::SCENE_HIST], nullptr);
		renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::SCENE_SRV]);
		renderer->GetContext()->PSSetShaderResources(depthTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::DEPTH_SRV]);
		eq->DrawStatic();
		UnBindShaderResourceViews();
		UnBindRenderTargets();
		END_EVENT();

		BEGIN_EVENT(L"Bloom pass")
			BEGIN_EVENT(L"Bloom Threshold")
			SetPipelineState(factory->GetState(EFFECTS_GLOW));
		renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM_1], nullptr);
		renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::SCENE_COLOR_SRV]);
		renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::MASK_SRV]);
		eq->DrawStatic();
		UnBindShaderResourceViews();
		UnBindRenderTargets();
		END_EVENT();

		BEGIN_EVENT(L"Blur pass");

		int kernel = 4;
		for (int i = 0; i < kernel; ++i) {
			SetPipelineState(factory->GetState(EFFECTS_VERTICAL_BLUR));
			renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM_2], nullptr);
			renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_1_SRV]);
			eq->DrawStatic();
			UnBindShaderResourceViews();
			UnBindRenderTargets();

			SetPipelineState(factory->GetState(EFFECTS_HORIZONTAL_BLUR));
			renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM_1], nullptr);
			renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_2_SRV]);
			eq->DrawStatic();
			UnBindShaderResourceViews();
			UnBindRenderTargets();
		}
		END_EVENT();

		BEGIN_EVENT(L"Bloom intensity")
			SetPipelineState(factory->GetState(EFFECTS_INTENSITY));
		renderer->GetContext()->ClearRenderTargetView(renderer->render_target_views[EffectsRTV::BLOOM_2], DirectX::Colors::Black);
		renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BLOOM_2], nullptr);
		renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_1_SRV]);
		eq->DrawStatic();
		UnBindShaderResourceViews();
		UnBindRenderTargets();
		END_EVENT();
		END_EVENT();

		if (is_first || !fxaa) {
			BEGIN_EVENT(L"FXAA pass")
				SetPipelineState(factory->GetState(EFFECTS_FXAA));
			renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::FXAA], nullptr);
			renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::MOTION_BLUR_SRV]);
			eq->DrawStatic();
			UnBindShaderResourceViews();
			UnBindRenderTargets();
			is_first = false;
		}
		else {
			BEGIN_EVENT(L"TAA pass")
				SetPipelineState(factory->GetState(EFFECTS_TAA));
			renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::FXAA], nullptr);

			if (renderer->index == 1) {
				renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::SCENE_HIST_SRV]);
				renderer->GetContext()->PSSetShaderResources(prevcolorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::MOTION_BLUR_SRV]);
				renderer->GetContext()->PSSetShaderResources(depthTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::DEPTH_HIST_SRV]);
				renderer->GetContext()->PSSetShaderResources(prevdepthTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::DEPTH_SRV]);
				renderer->GetContext()->PSSetShaderResources(velocityTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::VELOCITY_SRV]);
			}
			else {
				renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::MOTION_BLUR_SRV]);
				renderer->GetContext()->PSSetShaderResources(prevcolorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::SCENE_HIST_SRV]);
				renderer->GetContext()->PSSetShaderResources(depthTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::DEPTH_SRV]);
				renderer->GetContext()->PSSetShaderResources(prevdepthTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::DEPTH_HIST_SRV]);
				renderer->GetContext()->PSSetShaderResources(velocityTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::VELOCITY_SRV]);
			}
			eq->DrawStatic();
			UnBindShaderResourceViews();
			UnBindRenderTargets();
		}

	}

	SetPipelineState(factory->GetState(EFFECTS_SCENE));
	renderer->GetContext()->OMSetRenderTargets(1u, &renderer->render_target_views[EffectsRTV::BACKBUFFER], renderer->GetDepthStencilView(renderer->index));
	if (positions)
		renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::POSITIONS_SRV]);
	else if (normals)
		renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::NORMALS_SRV]);
	else if (albedo)
		renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::FXAA_SRV]);
	else 
		renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::FXAA_SRV]);
	renderer->GetContext()->PSSetShaderResources(bloomTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::BLOOM_2_SRV]);
	renderer->GetContext()->PSSetShaderResources(effectTexture.slot, 1u, &renderer->shader_resource_views[EffectsSRV::EFFECT_SRV]);
	eq->DrawStatic();
	UnBindShaderResourceViews();
	UnBindRenderTargets();
	END_EVENT();

	++renderer->index;
	renderer->index = renderer->index % 2;
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
	}if (defines & EFFECTS_TAA) {
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	}	
	if (defines & EFFECTS_MOTION_BLUR) {
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->dss = states->depth_stencil_states.at(DepthStencilState::NEVER);
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
