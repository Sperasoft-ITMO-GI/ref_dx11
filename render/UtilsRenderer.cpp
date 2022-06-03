#include <UtilsRenderer.h>

static D3D_SHADER_MACRO statMac[] = {
	"STATIC", "1", NULL, NULL
};
static D3D_SHADER_MACRO dynMac[] = {
	"DYNAMIC", "1", NULL, NULL
};
static D3D_SHADER_MACRO quadMac[] = {
	"QUAD", "1", NULL, NULL
};

static ShaderOptions statOpt{
	statMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};
static ShaderOptions dynOpt{
	dynMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions quadOpt{
	quadMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{UTILS_STATIC, statOpt},
	{UTILS_DYNAMIC, dynOpt},
	{UTILS_QUAD, quadOpt},
};

UtilsRenderer::~UtilsRenderer() {
	delete factory;
	delete p;
}

void UtilsRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\Utils.hlsl", new UtilsPSProvider, macro);
	p = new UtilsPoly();
	quad = new EffectsQuad();
	p->CreateDynamicVB(32);
	p->CreateDynamicIB(64);

	Renderer* renderer = Renderer::GetInstance();
	D3D11_TEXTURE2D_DESC texture_desc;
	ZeroMemory(&texture_desc, sizeof(D3D11_TEXTURE2D_DESC));
	texture_desc.Width = 128;
	texture_desc.Height = 128;
	texture_desc.MipLevels = 0;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
	ZeroMemory(&render_target_view_desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	render_target_view_desc.Format = texture_desc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	render_target_view_desc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
	ZeroMemory(&shader_resource_view_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shader_resource_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
	shader_resource_view_desc.Texture2D.MipLevels = 1;

	DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &renderer->test_tex));
	DXCHECK(renderer->GetDevice()->CreateRenderTargetView(renderer->test_tex, &render_target_view_desc, &renderer->test_rtv));
	DXCHECK(renderer->GetDevice()->CreateShaderResourceView(renderer->test_tex, &shader_resource_view_desc, &renderer->test_srv));
}

void UtilsRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	MODEL cb;
	MatrixBuffer mb;
	p->CreateCB(cb);
	quad->CreateCB(cb);
	cbuffer.Create(mb);

	SetPipelineState(factory->GetState(UTILS_QUAD));

	renderer->GetContext()->OMSetRenderTargets(
		1u,
		&renderer->test_rtv,
		nullptr
	);
	MatrixBuffer mbuffer;
	mbuffer.orthogonal = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 128, 128, 0.0f, 0.0f, 1000.0f));
	cbuffer.Update(mbuffer);
	cbuffer.Bind<MatrixBuffer>(buffer.slot);
	//quad->DrawStatic();
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

		//for (auto& poly : Utils_defs) {

			if (currentState != Utils_defs[0].flags)
			{
				if (Utils_defs[0].flags & UTILS_STATIC) {
					SetPipelineState(factory->GetState(UTILS_STATIC));
					currentState = UTILS_STATIC;
				}
			}

			p->UpdateDynamicVB(Utils_defs[0].vert);
			p->UpdateDynamicIB(Utils_defs[0].ind);
			p->UpdateCB(Utils_defs[0].cb);
			p->Draw();
		//}

		//Utils_defs.clear();
		temp.clear();
	}
}

void UtilsRenderer::RenderQuad()
{
	Renderer* renderer = Renderer::GetInstance();

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
	if (defines & UTILS_QUAD)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
		state->layout = MakeLayout(state->vs->GetBlob(), states->input_layouts.at(Layout::EFFECTS_QUAD));
		state->topology = states->topology.at(Topology::TRIANGLE_LISTS);
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