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
	{COLORED, colMac},
	{TEXTURED,  texMac},
	{FADE,  fadeMac},
};

void UIRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\UITexture.hlsl", new UIPSProvider(), macro);
}

void UIRenderer::Render() {
	for (auto& quad : quads) {
		if (quad.GetFlags() & COLORED) {
			SetPipelineState(factory->GetState(COLORED));
		}
		if (quad.GetFlags() & TEXTURED) {
			SetPipelineState(factory->GetState(TEXTURED));
		}
		if (quad.GetFlags() & FADE) {
			SetPipelineState(factory->GetState(FADE));
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
}

