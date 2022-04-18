#include <UtilsRenderer.h>

static D3D_SHADER_MACRO statMac[] = {
	"STATIC", "1", NULL, NULL
};
static D3D_SHADER_MACRO dynMac[] = {
	"DYNAMIC", "1", NULL, NULL
};

static ShaderOptions statOpt{
	statMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};
static ShaderOptions dynOpt{
	dynMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{UTILS_STATIC, statOpt},
	{UTILS_DYNAMIC, dynOpt},
};

UtilsRenderer::~UtilsRenderer() {
	delete factory;
	delete p;
}

void UtilsRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Utils.hlsl", new UtilsPSProvider, macro);
	p = new UtilsPoly();
	p->CreateDynamicVB(32);
	p->CreateDynamicIB(64);
}

void UtilsRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	MODEL cb;
	p->CreateCB(cb);
}

void UtilsRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	if (!Utils_defs.empty() && Utils_defs.back().flags & UTILS_DYNAMIC) {
		renderer->GetContext()->OMSetRenderTargets(
			1u,
			&renderer->render_target_views[EffectsRTV::SCENE],
			renderer->GetDepthStencilView(renderer->index)
		);
	}
	else {
		renderer->GetContext()->OMSetRenderTargets(
			1u,
			&renderer->render_target_views[EffectsRTV::BACKBUFFER],
			renderer->GetDepthStencilView(renderer->index)
		);
	}

	currentState = 0;

	if (!Utils_defs.empty() && Utils_defs.back().flags & UTILS_DYNAMIC) {
		SetPipelineState(factory->GetState(UTILS_DYNAMIC));
		p->UpdateDynamicVB(Utils_defs.back().vert);
		p->UpdateDynamicIB(Utils_defs.back().ind);
		p->UpdateCB(Utils_defs.back().cb);
		p->Draw();
		Utils_defs.pop_back();
	}
	else {
		for (int i = 0; i < Utils_defs.size(); ++i) {
			for (int j = i + 1; j < Utils_defs.size(); ++j) {
					if ((Utils_defs[i].vert[0].position.x - Utils_defs[j].vert[0].position.x) == 16 &&
						((Utils_defs[i].vert[0].position.z == Utils_defs[j].vert[0].position.z) ||
						 (Utils_defs[i].vert[0].position.z - Utils_defs[j].vert[0].position.z) < 5)) {
						//temp.push_back(Utils_defs[i]);
						for (int k = 0; k < 6; ++k) {
							Utils_defs[i].vert[k].position.x -= 8;
							Utils_defs[j].vert[k].position.x += 8;
						}
						break;
					}
					if ((Utils_defs[i].vert[0].position.y - Utils_defs[j].vert[0].position.y) == 16) {
						//temp.push_back(Utils_defs[i]);
						for (int k = 0; k < 6; ++k) {
							Utils_defs[i].vert[k].position.y -= 8;
							Utils_defs[j].vert[k].position.y += 8;
						}
						break;
					} 
			}
		}

		for (auto& poly : Utils_defs) {

			if (currentState != poly.flags)
			{
				if (poly.flags & UTILS_STATIC) {
					SetPipelineState(factory->GetState(UTILS_STATIC));
					currentState = UTILS_STATIC;
				}
			}

			p->UpdateDynamicVB(poly.vert);
			p->UpdateDynamicIB(poly.ind);
			p->UpdateCB(poly.cb);
			p->Draw();
		}

		Utils_defs.clear();
		temp.clear();
	}
}


void UtilsRenderer::Add(const UtilsDefinitions& polygon) {
	Utils_defs.push_back(polygon);
}

void UtilsRenderer::UtilsPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	if (defines & UTILS_STATIC)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_BACK);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::UTILS_POLYGON));
		state->topology = states->topology.at(Topology::LINE_LISTS);
	}	
	if (defines & UTILS_DYNAMIC)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_BACK);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::UTILS_POLYGON));
		state->topology = states->topology.at(Topology::LINE_LISTS);
	}

}


void UtilsRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new UtilsPSProvider(), macro);
}

bool UtilsRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void UtilsRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;

	factory_temp = nullptr;
}

void UtilsRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}