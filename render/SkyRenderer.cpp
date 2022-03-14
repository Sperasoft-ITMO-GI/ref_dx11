#include <SkyRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static ShaderOptions defOpt{
	defMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{SKY_DEFAULT, defOpt},
};

SkyRenderer::~SkyRenderer() {
	delete factory;
	delete sp;
}

void SkyRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Sky.hlsl", new SkyPSProvider(), macro);
	sp = new SkyCube();
}

void SkyRenderer::Render() {
	if (is_exist) {
		SetPipelineState(factory->GetState(SKY_DEFAULT));

		Renderer* renderer = Renderer::GetInstance();

		renderer->GetContext()->OMSetRenderTargets(
			1u,
			&renderer->render_target_views[EffectsRTV::BACKBUFFER],
			renderer->GetDepthStencilView()
		);

		renderer->BindSkyBox();

		sp->DrawStatic();
	}
}

void SkyRenderer::InitCB()
{
	Renderer* renderer = Renderer::GetInstance();
	MODEL cb;
	sp->CreateCB(cb);

	unsigned char** data = renderer->GetSkyBoxData();
	ID3D11Resource* res = nullptr;
	renderer->GetSkyBoxSRV()->GetResource(&res);
	for (int i = 0; i < 6; ++i) {
		renderer->GetContext()->UpdateSubresource(
			res,
			i, NULL,
			data[sky_box_order[i]],
			256 * (32 / 8), 0);
	}
}

void SkyRenderer::Add(MODEL& cbsb)
{
	sp->UpdateCB(cbsb);
}

void SkyRenderer::SkyPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	state->bs = states->blend_states.at(BlendState::NOBS);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_BACK);
	state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::SKY_POLYGON));
	state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
}


void SkyRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new SkyPSProvider(), macro);
}

bool SkyRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void SkyRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;
}

void SkyRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}
