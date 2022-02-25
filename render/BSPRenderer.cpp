#include <BSPRenderer.h>

static D3D_SHADER_MACRO alpMac[] = {
	"ALPHA", "1", NULL, NULL
};

static D3D_SHADER_MACRO watMac[] = {
	"WATER", "2", NULL, NULL
};

static D3D_SHADER_MACRO lightMappedPolyMac[] = {
	"LIGHTMAPPEDPOLY", "3", NULL, NULL
};

static ShaderOptions alpOpt{
	alpMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions watOpt{
	watMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions lightMappedPolyOpt{
	lightMappedPolyMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{BSP_ALPHA, alpOpt},
	{BSP_WATER, watOpt},
	{BSP_LIGHTMAPPEDPOLY, lightMappedPolyOpt}
};

BSPRenderer::~BSPRenderer()
{
	delete factory;
	delete p;
}

void BSPRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\BSP.hlsl", new ModelPSProvider, macro);
	p = new BSPPoly();
	p->CreateDynamicVB(32);
	p->CreateDynamicIB(64);
}

void BSPRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	ConstantBufferPolygon cbp;
	cbp.position_transform = renderer->GetModelView() * renderer->GetPerspective();
	p->CreateCB(cbp);
}

void BSPRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	currentState = 0;

	for (auto& poly : bsp_defs) {

		if (currentState != poly.flags)
		{
			if (poly.flags & BSP_ALPHA) {
				SetPipelineState(factory->GetState(BSP_ALPHA));
				currentState = BSP_ALPHA;
			}

			if (poly.flags & BSP_WATER) {
				SetPipelineState(factory->GetState(BSP_WATER));
				currentState = BSP_WATER;
			}

			if (poly.flags & BSP_LIGHTMAPPEDPOLY) {
				SetPipelineState(factory->GetState(BSP_LIGHTMAPPEDPOLY));
				currentState = BSP_LIGHTMAPPEDPOLY;
			}
		}
		
		renderer->Bind(poly.texture_index, 0);
		renderer->Bind(poly.lightmap_index, 1);

		p->UpdateDynamicVB(poly.vert);
		p->UpdateDynamicIB(poly.ind);
		p->UpdateCB(poly.cbp);
		p->DrawIndexed();
	}

	bsp_defs.clear();
}

void BSPRenderer::AddElement(const BSPPoly& polygon) {
	polygons.push_back(polygon);
}

void BSPRenderer::Add(const BSPDefinitions& polygon) {
	bsp_defs.push_back(polygon);
}

void BSPRenderer::ModelPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	if (defines & BSP_ALPHA)
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
	}

	if (defines & BSP_WATER)
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
	}

	if (defines & BSP_LIGHTMAPPEDPOLY)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
	}

	state->rs = states->rasterization_states.at(RasterizationState::CULL_FRONT);
	state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
	state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::BSP_POLYGON));
	state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
}


void BSPRenderer::InitNewFactory(const wchar_t* path)
{
	factory_temp = new PipelineFactory(path, new ModelPSProvider(), macro);
}

bool BSPRenderer::CompileWithDefines(int defines)
{
	bool error = false;
	factory_temp->GetState(defines, &error);

	if (error)
		return false;

	return true;
}

void BSPRenderer::ClearTempFactory()
{
	if (factory_temp != nullptr)
		delete factory_temp;

	factory_temp = nullptr;
}

void BSPRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}