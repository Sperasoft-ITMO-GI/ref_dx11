#include <EffectsRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static D3D_SHADER_MACRO sceneMac[] = {
	"SCENE", "1", NULL, NULL
};

static ShaderOptions defOpt{
	defMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions sceneOpt{
	sceneMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{EFFECTS_DEFAULT, defOpt},
	{EFFECTS_SCENE, sceneOpt},
};

EffectsRenderer::~EffectsRenderer() {
	delete factory;
	delete eq;
}

void EffectsRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Effects.hlsl", new EffectsPSProvider(), macro);
	eq = new EffectsQuad();

	Renderer* renderer = Renderer::GetInstance();
	/*auto window_params = renderer->GetWindowParameters();

	D3D11_TEXTURE2D_DESC texture_desc;
	ZeroMemory(&texture_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture_desc.Width = std::get<0>(window_params);
	texture_desc.Height = std::get<1>(window_params);
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &texture));

	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
	ZeroMemory(&render_target_view_desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	render_target_view_desc.Format = texture_desc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	render_target_view_desc.Texture2D.MipSlice = 0;

	DXCHECK(renderer->GetDevice()->CreateRenderTargetView(texture, &render_target_view_desc, &render_target));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
	ZeroMemory(&shader_resource_view_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shader_resource_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
	shader_resource_view_desc.Texture2D.MipLevels = 1;

	DXCHECK(renderer->GetDevice()->CreateShaderResourceView(texture, &shader_resource_view_desc, &resource_view));*/
}

void EffectsRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	// disable depth test
	renderer->GetContext()->OMSetRenderTargets(2, renderer->render_target_view, nullptr);

	SetPipelineState(factory->GetState(EFFECTS_DEFAULT));
	eq->DrawStatic();

	//renderer->GetContext()->OMSetRenderTargets(1, &(renderer->render_target_view[0]), nullptr);

	// unbind rtv[1]
	ID3D11RenderTargetView* rTargets[2] = { renderer->render_target_view[0], nullptr };
	renderer->GetContext()->OMSetRenderTargets(2, rTargets, nullptr);

	SetPipelineState(factory->GetState(EFFECTS_SCENE));
	//ID3D11RenderTargetView* tv = renderer->GetRenderTargetView(0);
	//renderer->GetContext()->OMSetRenderTargets(1, &tv, renderer->GetDepthStencilView());
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1, &(renderer->resource_view_rtv_1));
	eq->DrawStatic();

	// unbind srv
	ID3D11ShaderResourceView* nullSRV = { nullptr };
	renderer->GetContext()->PSSetShaderResources(colorTexture.slot, 1, &nullSRV);

	// enable depth test, bind all rtv's
	renderer->GetContext()->OMSetRenderTargets(2, renderer->render_target_view, renderer->GetDepthStencilView());
}

void EffectsRenderer::InitCB()
{
	Renderer* renderer = Renderer::GetInstance();
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
