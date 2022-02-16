#include <ParticlesRenderer.h>

static D3D_SHADER_MACRO solMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{PARTICLES_DEFAULT, solMac}
};

ParticlesRenderer::~ParticlesRenderer()
{
	delete factory;
	delete p;
}

void ParticlesRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Particles.hlsl", new ParticlesPSProvider, macro);
	p = new ParticlesPoly();
	p->CreateDynamicVB(20000);
	p->CreateDynamicIB(40000);
}

void ParticlesRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	ConstantBufferPolygon cbp;
	cbp.position_transform = renderer->GetPerspective();
	p->CreateCB(cbp);
}

void ParticlesRenderer::Render() {

	Renderer* renderer = Renderer::GetInstance();
	for (auto& poly : particles_defs) {
		if (poly.flags & PARTICLES_DEFAULT) {
			SetPipelineState(factory->GetState(PARTICLES_DEFAULT));
		}

		renderer->Bind(poly.texture_index, 0);

		p->UpdateDynamicVB(poly.vert);
		p->UpdateDynamicIB(poly.ind);
		p->UpdateCB(poly.cbp);
		p->DrawIndexed();
	}

	particles_defs.clear();
}

void ParticlesRenderer::Add(const ParticlesDefinitions& polygon) {
	particles_defs.push_back(polygon);
}

void ParticlesRenderer::ParticlesPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	if (defines & PARTICLES_DEFAULT)
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_BACK);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::PARTICLES_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
	}

}


void ParticlesRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new ParticlesPSProvider(), macro);
}

void ParticlesRenderer::CompileWithDefines(int defines)
{
	factory_temp->GetState(defines);
}

void ParticlesRenderer::ClearTempFactory()
{
	delete factory_temp;
}

void ParticlesRenderer::BindNewFactory()
{
	delete factory;
	factory = factory_temp;
}