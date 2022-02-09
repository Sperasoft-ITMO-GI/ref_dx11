#include <BeamRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{BEAM_DEFAULT, defMac},
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
	ConstantBufferPolygon cbp;
	cbp.position_transform = renderer->GetModelView() * renderer->GetPerspective();
	p->CreateCB(cbp);
}

void BeamRenderer::Render() {
	for (auto& poly : beam_defs) {
		if (poly.flags & BEAM_DEFAULT) {
			SetPipelineState(factory->GetState(BEAM_DEFAULT));
		}

		p->UpdateDynamicVB(poly.vert);
		p->UpdateCB(poly.cbp);
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
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_FRONT);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::BEAM_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_STRIP);
	}

}


void BeamRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new BeamPSProvider(), macro);
}

void BeamRenderer::CompileWithDefines(int defines)
{
	factory_temp->GetState(defines);
}

void BeamRenderer::ClearTempFactory()
{
	delete factory_temp;
}

void BeamRenderer::BindNewFactory()
{
	delete factory;
	factory = factory_temp;
}