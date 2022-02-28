#include <BeamRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static ShaderOptions defOpt{
	defMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{BEAM_DEFAULT, defOpt},
};

BeamRenderer::~BeamRenderer() {
	delete factory;
	delete p;
}

void BeamRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Beam.hlsl", new BeamPSProvider, macro);
	p = new BeamPoly();
	p->CreateDynamicVB(32);
	p->CreateDynamicIB(64);
}

void BeamRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	MODEL cb;
	p->CreateCB(cb);
}

void BeamRenderer::Render() {

	currentState = 0;

	for (auto& poly : beam_defs) {

		if (currentState != poly.flags)
		{
			if (poly.flags & BEAM_DEFAULT) {
				SetPipelineState(factory->GetState(BEAM_DEFAULT));
				currentState = BEAM_DEFAULT;
			}
		}

		p->UpdateDynamicVB(poly.vert);
		p->UpdateCB(poly.cb);
		p->Draw();
	}

	beam_defs.clear();
}


void BeamRenderer::Add(const BeamDefinitions& polygon) {
	beam_defs.push_back(polygon);
}

void BeamRenderer::BeamPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	if (defines & BEAM_DEFAULT)
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_BACK);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::BEAM_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_STRIP);
	}

}


void BeamRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new BeamPSProvider(), macro);
}

bool BeamRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void BeamRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;

	factory_temp = nullptr;
}

void BeamRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}