#include <UIRenderer.h>

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

UIRenderer::~UIRenderer() {
	delete factory;
	quad->~Quad();
}

void UIRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\UITexture.hlsl", new UIPSProvider(), macro);
	quad = new Quad(ConstantBufferQuad{});
}

void UIRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	for (auto& qd : qds) {
		if (qd.flags & UI_COLORED) {
			SetPipelineState(factory->GetState(UI_COLORED));
		}
		if (qd.flags & UI_TEXTURED) {
			SetPipelineState(factory->GetState(UI_TEXTURED));
		}
		if (qd.flags & UI_FADE) {
			SetPipelineState(factory->GetState(UI_FADE));
		}

		renderer->Bind(qd.teture_index);
		quad->UpdateCB(qd.cbq);
		quad->DrawStatic();
	}

	qds.clear();
}

void UIRenderer::Add(const QuadDefinitions& qd) {
	qds.push_back(qd);
}

void UIRenderer::UIPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	/* 
		For menu this states are similar
		but if for different defines we have to have different state  
		there are will be "if" statements to chose it
	*/
	States* states = States::GetInstance();

	state->bs = states->blend_states.at(BlendState::ALPHA);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
	state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::UI_QUAD));
	state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
}

