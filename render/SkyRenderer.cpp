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
	sp = new SkyPoly();
}

void SkyRenderer::Render() {
	if (is_exist) {
		SetPipelineState(factory->GetState(SKY_DEFAULT));

		Renderer* renderer = Renderer::GetInstance();
		/*if (!renderer->GetSkyBoxSRV())
			renderer->CreateSkyBoxSRV();*/
		renderer->BindSkyBox();

		sp->DrawStatic();
	}
}

void SkyRenderer::InitCB()
{
	Renderer* renderer = Renderer::GetInstance();
	ConstantBufferSkyBox cbsb;
	cbsb.position_transform = /*renderer->GetModelView() **/ renderer->GetPerspective();
	sp->CreateCB(cbsb);
}

void SkyRenderer::Add(ConstantBufferSkyBox& cbsb)
{
	sp->UpdateCB(cbsb);
}

//void SkyRenderer::Add(const SkyDefinitions& quad) {
//	sky_defs.push_back(quad);
//}

void SkyRenderer::SkyPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	/*
		For menu this states are similar
		but if for different defines we have to have different state
		there are will be "if" statements to chose it
	*/

	States* states = States::GetInstance();

	state->bs = states->blend_states.at(BlendState::NOBS);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
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
