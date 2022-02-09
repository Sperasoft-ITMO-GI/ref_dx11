#include <UIRenderer.h>

static D3D_SHADER_MACRO colMac[] = {
	"COLORED", "1", NULL, NULL
};
static D3D_SHADER_MACRO texMac[] = {
	"TEXTURED", "2", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{UI_COLORED, colMac},
	{UI_TEXTURED,  texMac}
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

	state->bs = states->blend_states.at(BlendState::UIALPHABS);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
	state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::UI_QUAD));
	state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
}

void UIRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new UIPSProvider(), macro);
}

void UIRenderer::CompileWithDefines(int defines)
{
	factory_temp->GetState(defines);
}

void UIRenderer::ClearTempFactory()
{
	delete factory_temp;
}

void UIRenderer::BindNewFactory()
{
	delete factory;
	factory = factory_temp;
}