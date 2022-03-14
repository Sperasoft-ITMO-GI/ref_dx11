#include <ParticlesRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static ShaderOptions defOpt{
	defMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY | GS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{PARTICLES_DEFAULT, defOpt}
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
	MODEL cb;
	p->CreateCB(cb);
}

void ParticlesRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	renderer->GetContext()->OMSetRenderTargets(
		1u,
		&renderer->render_target_views[EffectsRTV::BLOOM_1],
		renderer->GetDepthStencilView()
	);

	currentState = 0;

	for (auto& poly : particles_defs) {

		if (currentState != poly.flags)
		{
			if (poly.flags & PARTICLES_DEFAULT) {
				SetPipelineState(factory->GetState(PARTICLES_DEFAULT));
				currentState = PARTICLES_DEFAULT;
			}
		}

		p->UpdateCB(poly.cbp);
		p->UpdateDynamicVB(poly.vert);
		//p->UpdateDynamicIB(poly.ind);
		//p->DrawIndexed();

		p->Draw();
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
		state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::PARTICLES_POLYGON));
		state->topology = states->topology.at(Topology::POINT_LISTS);
	}

}


void ParticlesRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new ParticlesPSProvider(), macro);
}

bool ParticlesRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void ParticlesRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;

	factory_temp = nullptr;
}

void ParticlesRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}