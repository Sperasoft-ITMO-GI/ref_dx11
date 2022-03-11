#include <ModRenderer.h>

static D3D_SHADER_MACRO alpMac[] = {
	"ALPHA", "1", NULL, NULL
};

static D3D_SHADER_MACRO weaponMac[] = {
	"WEAPON", "1", NULL, NULL
};

static ShaderOptions defOpt{
	alpMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions weaponOpt{
	weaponMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{MOD_ALPHA, defOpt},
	{MOD_WEAPON, weaponOpt}
};

ModRenderer::~ModRenderer()
{
	delete factory;
	delete p;
}

void ModRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Model.hlsl", new ModelPSProvider, macro);
	p = new ModPoly();
	p->CreateDynamicVB(10000);
	p->CreateDynamicIB(10000);
}

void ModRenderer::InitCB() {
	MODEL cb;
	p->CreateCB(cb);
}

void ModRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	renderer->GetContext()->OMSetRenderTargets(
		1u,
		&renderer->render_target_views[EffectsRTV::SCENE], 
		renderer->GetDepthStencilView()
	);

	currentState = 0;

	for (auto& poly : mod_defs) {

		if (currentState != poly.flags)
		{
			if (poly.flags & MOD_ALPHA) {
				SetPipelineState(factory->GetState(MOD_ALPHA));
				currentState = MOD_ALPHA;
			}

			if (poly.flags & MOD_WEAPON) {
				SetPipelineState(factory->GetState(MOD_WEAPON));
				currentState = MOD_WEAPON;
			}
		}

		renderer->Bind(poly.texture_index, colorTexture.slot);
		renderer->Bind(poly.lightmap_index, lightmapTexture.slot);

		p->UpdateDynamicVB(poly.vert);
		p->UpdateDynamicIB(poly.ind);
		p->UpdateCB(poly.cb);
		p->DrawIndexed();
	}

	mod_defs.clear();
}

void ModRenderer::AddElement(const ModPoly& polygon) {
	polygons.push_back(polygon);
}

void ModRenderer::Add(const ModDefinitions& polygon) {
	//if (!mod_defs.empty()) {
	//	auto current_poly = std::find_if(mod_defs.begin(), mod_defs.end(),
	//		[&](const ModDefinitions& def) {
	//			return polygon.texture_index == def.texture_index;
	//		}
	//	);

	//	if (current_poly != mod_defs.end()) {
	//		for (auto& vert : polygon.vert) {
	//			current_poly->vert.push_back(vert);
	//		}
	//	}
	//	else {
	//		mod_defs.push_back(polygon);
	//	}
	//} else {
		mod_defs.push_back(polygon);
	//}
}

void ModRenderer::ModelPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	// Они разные только внутри шейдера
	if ((defines & MOD_ALPHA) || (defines & MOD_WEAPON))
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_BACK);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::MOD_POLYGON));
		state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
	}

}


void ModRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new ModelPSProvider(), macro);
}

bool ModRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void ModRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;

	factory_temp = nullptr;
}

void ModRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}