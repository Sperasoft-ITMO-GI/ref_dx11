#include <SkyRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{SKY_DEFAULT, defMac},
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
		renderer->BindSkyBox();

		sp->DrawStatic();
	}
}

void SkyRenderer::InitCB()
{
	Renderer* renderer = Renderer::GetInstance();
	ConstantBufferSkyBox cbsb;
	
	cbsb.position_transform = renderer->GetPerspective();
	sp->CreateCB(cbsb);

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

void SkyRenderer::Add(ConstantBufferSkyBox& cbsb)
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

void SkyRenderer::CompileWithDefines(int defines)
{
	factory_temp->GetState(defines);
}

void SkyRenderer::ClearTempFactory()
{
	delete factory_temp;
}

void SkyRenderer::BindNewFactory()
{
	delete factory;
	factory = factory_temp;
}
