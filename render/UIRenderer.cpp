#include <UIRenderer.h>

static D3D_SHADER_MACRO colMac[] = {
	"COLORED", "1", NULL, NULL
};
static D3D_SHADER_MACRO texMac[] = {
	"TEXTURED", "2", NULL, NULL
};

static ShaderOptions colOpt{
	colMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions texOpt{
	texMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{UI_COLORED, colOpt},
	{UI_TEXTURED,  texOpt}
};

UIRenderer::~UIRenderer() {
	delete factory;
	quad->~Quad();
}

void UIRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\UITexture.hlsl", new UIPSProvider(), macro);
	quad = new Quad(UI_BUFFER{});
}

void UIRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	currentState = 0;

	for (auto& qd : qds) {

		if (currentState != qd.flags)
		{
			if (qd.flags & UI_COLORED) {
				SetPipelineState(factory->GetState(UI_COLORED));
				currentState = UI_COLORED;
			}
			if (qd.flags & UI_TEXTURED) {
				SetPipelineState(factory->GetState(UI_TEXTURED));
				currentState = UI_TEXTURED;
			}
		}		

		renderer->Bind(qd.teture_index, colorTexture.slot);
		quad->UpdateCB(qd.cbq);
		quad->DrawStatic();
	}

	qds.clear();
}

void UIRenderer::Add(const QuadDefinitions& qd) {
	qds.push_back(qd);
}

void UIRenderer::UIPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	state->bs = states->blend_states.at(BlendState::UIALPHABS);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
	state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
	state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::UI_QUAD));
	state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
}

void UIRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new UIPSProvider(), macro);
}

bool UIRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void UIRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;
}

void UIRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}