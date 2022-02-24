#include <ModRenderer.h>

static D3D_SHADER_MACRO solMac[] = {
	"ALPHA", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{MOD_ALPHA, solMac}
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
	for (auto& poly : mod_defs) {
		if (poly.flags & MOD_ALPHA) {
			SetPipelineState(factory->GetState(MOD_ALPHA));
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

void ModRenderer::CompileWithDefines(int defines)
{
	factory_temp->GetState(defines);
}

void ModRenderer::ClearTempFactory()
{
	delete factory_temp;
}

void ModRenderer::BindNewFactory()
{
	delete factory;
	factory = factory_temp;
}