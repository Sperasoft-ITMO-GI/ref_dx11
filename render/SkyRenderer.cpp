#include <SkyRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{SKY_DEFAULT, defMac},
};

SkyRenderer::~SkyRenderer() {
	delete factory;
	delete q;
}

void SkyRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Sky.hlsl", new SkyPSProvider(), macro);
	q = new Quad(ConstantBufferQuad{});
	q->CreateDynamicVB(4);
	q->CreateDynamicIB(6);
}

void SkyRenderer::Render() {
	/*for (auto& quad : quads) {
		if (quad.GetFlags() & SKY_DEFAULT) {
			SetPipelineState(factory->GetState(SKY_DEFAULT));
		}

		Draw(quad);
	}

	quads.clear();*/
	Renderer* renderer = Renderer::GetInstance();
	for (auto& poly : sky_defs) {
		if (poly.flags & SKY_DEFAULT) {
			SetPipelineState(factory->GetState(SKY_DEFAULT));
		}

		renderer->Bind(poly.texture_index);
		q->UpdateDynamicVB(poly.vert);
		q->UpdateDynamicIB(poly.ind);
		q->UpdateCB(poly.cbq);
		q->DrawIndexed();
	}

	sky_defs.clear();
}

void SkyRenderer::AddElement(const Quad& quad) {
	quads.push_back(quad);
}

void SkyRenderer::Add(const SkyDefinitions& quad) {
	sky_defs.push_back(quad);
}

void SkyRenderer::SkyPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	/*
		For menu this states are similar
		but if for different defines we have to have different state
		there are will be "if" statements to chose it
	*/

	States* states = States::GetInstance();

	state->bs = states->blend_states.at(BlendState::ALPHA);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_FRONT);
	state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::SKY_QUAD));
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
