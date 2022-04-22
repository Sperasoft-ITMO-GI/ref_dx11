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

static D3D_SHADER_MACRO texMac[] = {
	"TEX", "4", NULL, NULL
};

static D3D_SHADER_MACRO lmtexMac[] = {
	"LMTEX", "5", NULL, NULL
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

static ShaderOptions texOpt{
	texMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions lmtexOpt{
	lmtexMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{BSP_ALPHA, alpOpt},
	{BSP_WATER, watOpt},
	{BSP_LIGHTMAPPEDPOLY, lightMappedPolyOpt},
	{BSP_TEX, texOpt},
	{BSP_LMTEX, lmtexOpt}
};

BSPRenderer::~BSPRenderer()
{
	delete factory;
	delete p;
}

void BSPRenderer::Init() {
	factory = new PipelineFactory(L"ref_dx11\\shaders\\BSP.hlsl", new ModelPSProvider, macro);
	p = new BSPPoly();
	p->CreateDynamicVB(64);
	p->CreateDynamicIB(256);

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

	DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &texs[0]));
	DXCHECK(renderer->GetDevice()->CreateRenderTargetView(texs[0], &render_target_view_desc, &views[0]));
	DXCHECK(renderer->GetDevice()->CreateShaderResourceView(texs[0], &shader_resource_view_desc, &srv[0]));

	texture_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	render_target_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.Format = texture_desc.Format;
	DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &texs[1]));	
	DXCHECK(renderer->GetDevice()->CreateRenderTargetView(texs[1], &render_target_view_desc, &views[1]));
	DXCHECK(renderer->GetDevice()->CreateShaderResourceView(texs[1], &shader_resource_view_desc, &srv[1]));
	
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	render_target_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.Format = texture_desc.Format;
	DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &texs[2]));
	DXCHECK(renderer->GetDevice()->CreateRenderTargetView(texs[2], &render_target_view_desc, &views[2]));
	DXCHECK(renderer->GetDevice()->CreateShaderResourceView(texs[2], &shader_resource_view_desc, &srv[2]));

}

void BSPRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	MODEL cb;
	p->CreateCB(cb);
	MatrixBuffer mb;
	mat_buf.Create(mb);
}

void BSPRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	if (is_gbuffer) {
		renderer->GetContext()->ClearRenderTargetView(views[0], DirectX::Colors::Black);
		renderer->GetContext()->ClearRenderTargetView(views[1], DirectX::Colors::Black);
		renderer->GetContext()->ClearRenderTargetView(views[2], DirectX::Colors::Black);

		if (renderer->index == 0) {
			renderer->GetContext()->OMSetRenderTargets(
				1u,
				&renderer->test_rtv,
				nullptr
			);
		}
		else {
			renderer->GetContext()->OMSetRenderTargets(
				1u,
				&renderer->test_rtv,
				nullptr
			);
		}

		currentState = 0;

		for (auto& poly : bsp_defs2d) {

			if (currentState != poly.flags)
			{
				SetPipelineState(factory->GetState(BSP_TEX));
			}

			renderer->Bind(poly.texture_index, colorTexture.slot);
			renderer->Bind(poly.lightmap_index, lightmapTexture.slot);

			p->UpdateDynamicVB(poly.vert);
			p->UpdateDynamicIB(poly.ind);
			p->UpdateCB(poly.cb);
			mat_buf.Update(poly.mb);

			mat_buf.Bind<MatrixBuffer>(buffer.slot);
			p->DrawIndexed();
		}
		bsp_defs2d.clear();

		renderer->GetContext()->ClearRenderTargetView(renderer->render_target_views[SCENE_COLOR], DirectX::Colors::Black);
		ID3D11ShaderResourceView* null_srv[1] = { nullptr };
		renderer->GetContext()->PSSetShaderResources(lightmapTexture.slot, 1u, null_srv);
		renderer->GetContext()->OMSetRenderTargets(
			1u,
			&renderer->render_target_views[SCENE_COLOR],
			nullptr
		);

		renderer->GetContext()->PSSetShaderResources(lightmapTexture.slot, 1U, &renderer->test_srv);
		for (auto& poly : bsp_defs2d2) {

			if (currentState != poly.flags)
			{
				SetPipelineState(factory->GetState(BSP_LMTEX));
			}

			//renderer->Bind(poly.texture_index, colorTexture.slot);
			//renderer->Bind(poly.lightmap_index, lightmapTexture.slot);

			p->UpdateDynamicVB(poly.vert);
			p->UpdateDynamicIB(poly.ind);
			p->UpdateCB(poly.cb);
			mat_buf.Update(poly.mb);

			mat_buf.Bind<MatrixBuffer>(buffer.slot);
			p->DrawIndexed();
		}
		bsp_defs2d2.clear();
	}
	else {
		if (renderer->index == 0) {
			ID3D11RenderTargetView* render_targets[5] = {
				renderer->render_target_views[EffectsRTV::SCENE],
				renderer->render_target_views[EffectsRTV::MASK],
				renderer->render_target_views[EffectsRTV::VELOSITY],
				renderer->render_target_views[EffectsRTV::LIGHTMAP],
				renderer->render_target_views[EffectsRTV::SCENE_COLOR],
			};

			renderer->GetContext()->OMSetRenderTargets(
				5u,
				render_targets,
				renderer->GetDepthStencilView(renderer->index)
			);
		}
		else {
			ID3D11RenderTargetView* render_targets[5] = {
				renderer->render_target_views[EffectsRTV::SCENE],
				renderer->render_target_views[EffectsRTV::MASK],
				renderer->render_target_views[EffectsRTV::VELOSITY],
				renderer->render_target_views[EffectsRTV::LIGHTMAP],
				renderer->render_target_views[EffectsRTV::SCENE_COLOR],
			};

			renderer->GetContext()->OMSetRenderTargets(
				5u,
				render_targets,
				renderer->GetDepthStencilView(renderer->index)
			);
		}

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

			renderer->Bind(poly.texture_index, colorTexture.slot);
			renderer->Bind(poly.lightmap_index, lightmapTexture.slot);

			p->UpdateDynamicVB(poly.vert);
			p->UpdateDynamicIB(poly.ind);
			p->UpdateCB(poly.cb);
			mat_buf.Update(poly.mb);

			mat_buf.Bind<MatrixBuffer>(buffer.slot);
			p->DrawIndexed();
		}

		bsp_defs.clear();
	}





}

void BSPRenderer::AddElement(const BSPPoly& polygon) {
	polygons.push_back(polygon);
}

void BSPRenderer::Add(const BSPDefinitions& polygon) {
	//if (!bsp_defs.empty()) {
	//	auto current_poly = std::find_if(bsp_defs.begin(), bsp_defs.end(), 
	//		[&](const BSPDefinitions& def) {
	//			return polygon.texture_index == def.texture_index;
	//		}
	//	);

	//	if (current_poly != bsp_defs.end()) {
	//		for (auto& vert : polygon.vert) {
	//			current_poly->vert.push_back(vert);
	//		}
	//	} else {
	//		bsp_defs.push_back(polygon);
	//	}
	//} else {
		bsp_defs.push_back(polygon);
	//}
}

void BSPRenderer::Add2d(const BSPDefinitions& polygon)
{
	bsp_defs2d.push_back(polygon);
}

void BSPRenderer::Add2d2(const BSPDefinitions& polygon)
{
	bsp_defs2d2.push_back(polygon);
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

	//if (defines & BSP_TEX)
	//{
	//	state->bs = states->blend_states.at(BlendState::NOBS);
	//	state->dss = states->depth_stencil_states.at(DepthStencilState::NEVER);
	//}

	state->rs = states->rasterization_states.at(RasterizationState::CULL_FRONT);
	state->dss = states->depth_stencil_states.at(DepthStencilState::LESS_EQUAL);
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