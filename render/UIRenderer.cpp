#include <UIRenderer.h>

static std::vector<D3D_SHADER_MACRO> sm{
	{"COLORED", "1"},
	{"TEXTURED", "2"},
	{"NULL", "NULL"}
};

static D3D_SHADER_MACRO macro[] = {
	"TEXTURED", "2", "NULL", "NULL"
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

		quad.Draw();
	}

	quads.clear();
}

void UIRenderer::Swap() {
	Renderer* renderer = Renderer::GetInstance();
	renderer->GetSwapChain()->Present(1, 0);
	
}

void UIRenderer::AddElement(const Quad& quad) {
	quads.push_back(quad);
}

void UIRenderer::UIPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	state->dss = DepthStencilState::DEFAULT;
	state->bs = BlendState::ALPHA;
	state->rs = RasterizationState::CULL_NONE;

	if (defines & COLORED) {
		state->layout = Layout::COLOR;
	}
	if (defines & TEXTURED) {
		state->layout = Layout::TEXTURE;
	}
}

