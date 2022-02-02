#include <SkyRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{DEFAULT, defMac},
};

void SkyRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Sky.hlsl", new SkyPSProvider(), macro);
}

void SkyRenderer::Render() {
	for (auto& quad : quads) {
		if (quad.GetFlags() & DEFAULT) {
			SetPipelineState(factory->GetState(DEFAULT));
		}

		quad.Draw();
	}

	quads.clear();
}

void SkyRenderer::AddElement(const SkyQuad& quad) {
	quads.push_back(quad);
}

void SkyRenderer::SkyPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	/*
		For menu this states are similar
		but if for different defines we have to have different state
		there are will be "if" statements to chose it
	*/

	state->bs = BlendState::ALPHA;
	state->rs = RasterizationState::CULL_NONE;
	state->layout = Layout::SKY_QUAD;
	state->topology = Topology::TRIANGLE_LISTS;
}

