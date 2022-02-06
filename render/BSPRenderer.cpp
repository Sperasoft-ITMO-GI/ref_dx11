#include <BSPRenderer.h>

// Тут нужно заменить дефайны на необходимые (это копипаста UIRenderer)

static D3D_SHADER_MACRO solMac[] = {
	"SOLID", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{BSP_SOLID, solMac}
};

void  BSPRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\BSP.hlsl", new ModelPSProvider, macro);
}

void BSPRenderer::Render() {
	for (auto& polygon : polygons) {

		if (polygon.GetFlags() & BSP_SOLID) {
			SetPipelineState(factory->GetState(BSP_SOLID));
		}

		Draw(polygon);
	}

	polygons.clear();
}

void BSPRenderer::AddElement(const BSPPoly& polygon) {
	polygons.push_back(polygon);
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