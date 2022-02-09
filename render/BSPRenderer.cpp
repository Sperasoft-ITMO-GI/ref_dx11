#include <BSPRenderer.h>

static D3D_SHADER_MACRO solMac[] = {
	"SOLID", "1", NULL, NULL
};

static D3D_SHADER_MACRO alpMac[] = {
	"ALPHA", "1", NULL, NULL
};

static D3D_SHADER_MACRO watMac[] = {
	"WATER", "1", NULL, NULL
};

static D3D_SHADER_MACRO lightMac[] = {
	"LIGHTMAP", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{BSP_SOLID, solMac},
	{BSP_ALPHA, alpMac},
	{BSP_WATER, watMac},
	{BSP_LIGHTMAP, lightMac}
};

BSPRenderer::~BSPRenderer()
{
	delete factory;
	delete p;
}

void BSPRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\BSP.hlsl", new ModelPSProvider, macro);
	p = new BSPPoly();
	p->CreateDynamicVB(32);
	p->CreateDynamicIB(64);
}

void BSPRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	ConstantBufferPolygon cbp;
	cbp.position_transform = renderer->GetModelView() * renderer->GetPerspective();
	p->CreateCB(cbp);
}

void BSPRenderer::Render() {

	Renderer* renderer = Renderer::GetInstance();
	for (auto& poly : bsp_defs) {
		if (poly.flags & BSP_SOLID) {
			SetPipelineState(factory->GetState(BSP_SOLID));
		}

		if (poly.flags & BSP_ALPHA) {
			SetPipelineState(factory->GetState(BSP_ALPHA));
		}

		if (poly.flags & BSP_WATER) {
			SetPipelineState(factory->GetState(BSP_WATER));
		}

		if (poly.flags & BSP_LIGHTMAP) {
			SetPipelineState(factory->GetState(BSP_LIGHTMAP));
		}
		
		renderer->Bind(poly.texture_index);
		p->UpdateDynamicVB(poly.vert);
		p->UpdateDynamicIB(poly.ind);
		p->UpdateCB(poly.cbp);
		p->DrawIndexed();
	}

	bsp_defs.clear();
}

void BSPRenderer::AddElement(const BSPPoly& polygon) {
	polygons.push_back(polygon);
}

void BSPRenderer::Add(const BSPDefinitions& polygon) {
	bsp_defs.push_back(polygon);
}

void BSPRenderer::ModelPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	if (defines & BSP_SOLID)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_FRONT);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::BSP_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
	}

	if (defines & BSP_ALPHA)
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_FRONT);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::BSP_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
	}

	if (defines & BSP_WATER)
	{
		state->bs = states->blend_states.at(BlendState::WATERBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_FRONT);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::BSP_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_STRIP);
	}

	if (defines & BSP_LIGHTMAP)
	{
		state->bs = states->blend_states.at(BlendState::SURFLIGHTMAPBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_FRONT);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::BSP_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
	}
	
}


void BSPRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new ModelPSProvider(), macro);
}

void BSPRenderer::CompileWithDefines(int defines)
{
	factory_temp->GetState(defines);
}

void BSPRenderer::ClearTempFactory()
{
	delete factory_temp;
}

void BSPRenderer::BindNewFactory()
{
	delete factory;
	factory = factory_temp;
}