#include <BSPRenderer.h>

// Тут нужно заменить дефайны на необходимые (это копипаста UIRenderer)

static D3D_SHADER_MACRO solMac[] = {
	"SOLID", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{BSP_SOLID, solMac}
};

BSPRenderer::~BSPRenderer()
{
	delete factory;
	p->~BSPPoly();
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
	//for (auto& polygon : polygons) {

	//	if (polygon.GetFlags() & BSP_SOLID) {
	//		SetPipelineState(factory->GetState(BSP_SOLID));
	//	}

	//	Draw(polygon);
	//}

	//polygons.clear();
	Renderer* renderer = Renderer::GetInstance();
	for (auto& poly : bsp_defs) {
		if (poly.flags & BSP_SOLID) {
			SetPipelineState(factory->GetState(BSP_SOLID));
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
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
	}
	
}