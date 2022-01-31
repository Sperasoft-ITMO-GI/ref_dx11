#include <UIRenderer.h>

//static std::vector<D3D_SHADER_MACRO> sm{
//	{"COLORED", "1"},
//	{"TEXTURED", "2"},
//	{"NULL", "NULL"}
//};

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
	{UI_COLORED, colMac},
	{UI_TEXTURED,  texMac},
	{UI_FADE,  fadeMac}
};

void UIRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\UITexture.hlsl", new UIPSProvider(), macro);
}

void UIRenderer::Render() {
	for (auto& quad : quads) {
		if (quad.GetFlags() & UI_COLORED) {
			SetPipelineState(factory->GetState(UI_COLORED));
		}
		if (quad.GetFlags() & UI_TEXTURED) {
			SetPipelineState(factory->GetState(UI_TEXTURED));
		}
		if (quad.GetFlags() & UI_FADE) {
			SetPipelineState(factory->GetState(UI_FADE));
		}

		quad.Draw();
	}

	quads.clear();
}

void UIRenderer::AddElement(const Quad& quad) {
	quads.push_back(quad);
}

void UIRenderer::UIPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	/* 
		For menu this states are similar
		but if for different defines we have to have different state  
		there are will be "if" statements to chose it
	*/

	state->dss = DepthStencilState::DEFAULT;
	state->bs = BlendState::ALPHA;
	state->rs = RasterizationState::CULL_NONE;
	state->layout = Layout::QUAD;
	state->topology = Topology::TRIANGLE_LISTS;
}

