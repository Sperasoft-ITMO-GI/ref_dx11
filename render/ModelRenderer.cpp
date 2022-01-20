#include <ModelRenderer.h>

// Тут нужно заменить дефайны на необходимые (это копипаста UIRenderer)

static D3D_SHADER_MACRO colMac[] = {
	"COLORED", "1", NULL, NULL
};
static D3D_SHADER_MACRO texMac[] = {
	"TEXTURED", "2", NULL, NULL
};
static D3D_SHADER_MACRO fadeMac[] = {
	"FADE", "4", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{COLORED, colMac},
	{TEXTURED,  texMac},
	{FADE,  fadeMac},
};

void  ModelRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\ModelShader.hlsl", new ModelPSProvider, macro);
}

void ModelRenderer::Render() {
	for (auto& polygon : polygons) {
		if (polygon.GetFlags() & COLORED) {
			SetPipelineState(factory->GetState(COLORED));
		}
		if (polygon.GetFlags() & TEXTURED) {
			SetPipelineState(factory->GetState(TEXTURED));
		}
		if (polygon.GetFlags() & FADE) {
			SetPipelineState(factory->GetState(FADE));
		}

		polygon.Draw();
	}

	polygons.clear();
}

void ModelRenderer::AddElement(const BSPPoly& polygon) {
	polygons.push_back(polygon);
}

void ModelRenderer::ModelPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	state->dss = DepthStencilState::DEFAULT;
	state->bs = BlendState::ALPHA;
	state->rs = RasterizationState::CULL_BACK;
	state->layout = Layout::POLYGON;
}