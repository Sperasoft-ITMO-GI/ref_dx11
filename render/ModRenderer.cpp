#include <ModRenderer.h>

static D3D_SHADER_MACRO alpMac[] = {
	"ALPHA", "1", NULL, NULL
};

static ShaderOptions defOpt{
	alpMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{MOD_ALPHA, defOpt}
};

ModRenderer::~ModRenderer()
{
	delete factory;
	delete p;
}

void ModRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Model.hlsl", new ModelPSProvider, macro);
	p = new ModPoly();
	p->CreateDynamicVB(64);
	p->CreateDynamicIB(128);
}

void ModRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	ConstantBufferPolygon cbp;
	cbp.position_transform = renderer->GetModelView() * renderer->GetPerspective();
	p->CreateCB(cbp);
}

void ModRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	currentState = 0;

	for (auto& poly : mod_defs) {

		if (currentState != poly.flags)
		{
			if (poly.flags & MOD_ALPHA) {
				SetPipelineState(factory->GetState(MOD_ALPHA));
				currentState = MOD_ALPHA;
			}
		}		

		renderer->Bind(poly.texture_index, 0);
		renderer->Bind(poly.lightmap_index, 1);

		p->UpdateDynamicVB(poly.vert);
		p->UpdateDynamicIB(poly.ind);
		p->UpdateCB(poly.cbp);
		p->DrawIndexed();
	}

	mod_defs.clear();
}

void ModRenderer::AddElement(const ModPoly& polygon) {
	polygons.push_back(polygon);
}

void ModRenderer::Add(const ModDefinitions& polygon) {
	mod_defs.push_back(polygon);
}

void ModRenderer::ModelPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	if (defines & MOD_ALPHA)
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_BACK);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::MOD_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
	}

}


void ModRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new ModelPSProvider(), macro);
}

bool ModRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void ModRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;

	factory_temp = nullptr;
}

void ModRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}