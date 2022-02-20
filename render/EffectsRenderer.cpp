#include <EffectsRenderer.h>

static D3D_SHADER_MACRO defMac[] = {
	"DEFAULT", "1", NULL, NULL
};

static std::unordered_map<int, D3D_SHADER_MACRO*> macro{
	{EFFECTS_DEFAULT, defMac},
};

EffectsRenderer::~EffectsRenderer() {
	delete factory;
	delete eq;
}

void EffectsRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Effects.hlsl", new EffectsPSProvider(), macro);
	eq = new EffectsQuad();
}

void EffectsRenderer::Render() {
	SetPipelineState(factory->GetState(EFFECTS_DEFAULT));

	eq->DrawStatic();
}

void EffectsRenderer::InitCB()
{
	Renderer* renderer = Renderer::GetInstance();
	ConstantBufferEffects cbe;

	cbe.position_transform = 
		 DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(90))
		* DirectX::XMMatrixTranslation(10, 100, 100)
		* renderer->GetModelView()
		* renderer->GetPerspective();
	eq->CreateCB(cbe);
}

void EffectsRenderer::Add(ConstantBufferEffects& cbe)
{
	eq->UpdateCB(cbe);
}

void EffectsRenderer::EffectsPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	state->bs = states->blend_states.at(BlendState::ALPHABS);
	state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
	state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::EFFECTS_QUAD));
	state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
}


void EffectsRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new EffectsPSProvider(), macro);
}

void EffectsRenderer::CompileWithDefines(int defines)
{
	factory_temp->GetState(defines);
}

void EffectsRenderer::ClearTempFactory()
{
	delete factory_temp;
}

void EffectsRenderer::BindNewFactory()
{
	delete factory;
	factory = factory_temp;
}
