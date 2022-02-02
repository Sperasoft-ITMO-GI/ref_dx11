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

		polygon.Draw();
	}

	polygons.clear();
}

void BSPRenderer::AddElement(const BSPPoly& polygon) {
	polygons.push_back(polygon);
}

void BSPRenderer::ModelPSProvider::PatchPipelineState(PipelineState* state, int defines) {

	if (defines & BSP_SOLID)
	{
		state->bs = BlendState::NOALPHA;
		state->rs = RasterizationState::CULL_NONE;
		state->layout = Layout::POLYGON;
		state->topology = Topology::TRIANGLE_STRIP; // I'm not sure
	}
	
}