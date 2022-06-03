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

static D3D_SHADER_MACRO texWireframeMac[] = {
	"TEX", "5", NULL, NULL
};

static D3D_SHADER_MACRO texLightmapMac[] = {
	"LIGHTMAP", "6", NULL, NULL
};

static D3D_SHADER_MACRO texLightmapWireMac[] = {
	"LIGHTMAP", "7", NULL, NULL
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

static ShaderOptions texWireframeOpt{
	texWireframeMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions texLightmapOpt{
	texLightmapMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static ShaderOptions texLightmapWireOpt{
	texLightmapWireMac,
	PS_SHADER_ENTRY | VS_SHADER_ENTRY
};

static std::unordered_map<int, ShaderOptions> macro{
	{BSP_ALPHA, alpOpt},
	{BSP_WATER, watOpt},
	{BSP_LIGHTMAPPEDPOLY, lightMappedPolyOpt},
	{BSP_TEX, texOpt},
	{BSP_TEX_WIREFRAME, texWireframeOpt},
	{BSP_TEX_LIGHTMAP, texLightmapOpt},
	{BSP_TEX_LIGHTMAPWIRE, texLightmapWireOpt},
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

	for (int i = 0; i < 32; ++i) {
		DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &pos_texs[i]));
		DXCHECK(renderer->GetDevice()->CreateRenderTargetView(pos_texs[i], &render_target_view_desc, &pos_rtv[i]));
		DXCHECK(renderer->GetDevice()->CreateShaderResourceView(pos_texs[i], &shader_resource_view_desc, &pos_srv[i]));
	}

	texture_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	render_target_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.Format = texture_desc.Format;
	for (int i = 0; i < 32; ++i) {
		DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &norm_texs[i]));
		DXCHECK(renderer->GetDevice()->CreateRenderTargetView(norm_texs[i], &render_target_view_desc, &norm_rtv[i]));
		DXCHECK(renderer->GetDevice()->CreateShaderResourceView(norm_texs[i], &shader_resource_view_desc, &norm_srv[i]));
		DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &lightmap_texs[i]));
		DXCHECK(renderer->GetDevice()->CreateRenderTargetView(lightmap_texs[i], &render_target_view_desc, &lightmap_rtv[i]));
		DXCHECK(renderer->GetDevice()->CreateShaderResourceView(lightmap_texs[i], &shader_resource_view_desc, &lightmap_srv[i]));
	}

	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	render_target_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.Format = texture_desc.Format;
	for (int i = 0; i < 32; ++i) {
		DXCHECK(renderer->GetDevice()->CreateTexture2D(&texture_desc, nullptr, &alb_texs[i]));
		DXCHECK(renderer->GetDevice()->CreateRenderTargetView(alb_texs[i], &render_target_view_desc, &alb_rtv[i]));
		DXCHECK(renderer->GetDevice()->CreateShaderResourceView(alb_texs[i], &shader_resource_view_desc, &alb_srv[i]));
	}

	viewports[0].TopLeftX = 0.0f;
	viewports[0].TopLeftY = 0.0f;
	viewports[0].Width = 128.0f;
	viewports[0].Height = 128.0f;
	viewports[0].MinDepth = 0.0f;
	viewports[0].MaxDepth = 1.0f;

	viewports[1].TopLeftX = 0.0f;
	viewports[1].TopLeftY = 0.0f;
	viewports[1].Width = std::get<0>(renderer->GetWindowParameters());
	viewports[1].Height = std::get<1>(renderer->GetWindowParameters());;
	viewports[1].MinDepth = 0.0f;
	viewports[1].MaxDepth = 1.0f;

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.BorderColor[0] = 1.0f;
	sampler_desc.BorderColor[1] = 1.0f;
	sampler_desc.BorderColor[2] = 1.0f;
	sampler_desc.BorderColor[3] = 1.0f;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MinLOD = -FLT_MAX;
	sampler_desc.MaxLOD = FLT_MAX;
	renderer->GetDevice()->CreateSamplerState(&sampler_desc, &samplers[0]);

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.BorderColor[0] = 1.0f;
	sampler_desc.BorderColor[1] = 1.0f;
	sampler_desc.BorderColor[2] = 1.0f;
	sampler_desc.BorderColor[3] = 1.0f;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MinLOD = -FLT_MAX;
	sampler_desc.MaxLOD = FLT_MAX;
	renderer->GetDevice()->CreateSamplerState(&sampler_desc, &samplers[1]);

}

void BSPRenderer::InitCB() {
	Renderer* renderer = Renderer::GetInstance();
	MODEL cb;
	p->CreateCB(cb);
	MatrixBuffer mb;
	mat_buf.Create(mb);
	LightSources ls;
	light_sources_buffer.Create(ls);
}

void BSPRenderer::Render() {
	Renderer* renderer = Renderer::GetInstance();

	if (is_first) {
		LightSources ls;
		ls.point_light = point_light_buf;
		ls.sources_count = light_source_index;
		for (int i = 0; i < light_source_index; ++i) {
			ls.source[i] = light_sources[i];
		}
		light_sources_buffer.Update(ls);

		currentState = 0;

		renderer->GetContext()->PSSetSamplers(0, 1, &samplers[0]);
		renderer->GetContext()->RSSetViewports(1, &viewports[0]);

		for (auto& poly : bsp_defs2d) {

			ID3D11RenderTargetView* render_targets[3] = {
				pos_rtv[poly.lightmap_index],
				norm_rtv[poly.lightmap_index],
				alb_rtv[poly.lightmap_index]
			};
			renderer->GetContext()->OMSetRenderTargets(
				3u,
				render_targets,
				nullptr
			);

			if (currentState != poly.flags)
			{
				if (poly.flags & BSP_TEX) {
					SetPipelineState(factory->GetState(BSP_TEX));
					currentState = BSP_TEX;
				}
				if (poly.flags & BSP_TEX_WIREFRAME){
					SetPipelineState(factory->GetState(BSP_TEX_WIREFRAME));
					currentState = BSP_TEX_WIREFRAME;
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

		bsp_defs2d.clear();
		is_first = false;
	}
	else if (is_lightmap) {
		currentState = 0;

		renderer->GetContext()->PSSetSamplers(0, 1, &samplers[0]);
		renderer->GetContext()->RSSetViewports(1, &viewports[0]);

		for (auto& poly : bsp_defs2d) {
			renderer->GetContext()->OMSetRenderTargets(
				1u,
				&lightmap_rtv[poly.lightmap_index],
				nullptr
			);

			if (currentState != poly.flags)
			{
				if (poly.flags & BSP_TEX_LIGHTMAP) {
					SetPipelineState(factory->GetState(BSP_TEX_LIGHTMAP));
					currentState = BSP_TEX_LIGHTMAP;
				}
				if (poly.flags & BSP_TEX_LIGHTMAPWIRE) {
					SetPipelineState(factory->GetState(BSP_TEX_LIGHTMAPWIRE));
					currentState = BSP_TEX_LIGHTMAPWIRE;
				}
			}

			renderer->GetContext()->PSSetShaderResources(positionTexture.slot, 1, &pos_srv[poly.lightmap_index]);
			renderer->GetContext()->PSSetShaderResources(normalTexture.slot, 1, &norm_srv[poly.lightmap_index]);
			renderer->GetContext()->PSSetShaderResources(albedoTexture.slot, 1, &alb_srv[poly.lightmap_index]);

			p->UpdateDynamicVB(poly.vert);
			p->UpdateDynamicIB(poly.ind);
			p->UpdateCB(poly.cb);
			mat_buf.Update(poly.mb);

			mat_buf.Bind<MatrixBuffer>(buffer.slot);
			light_sources_buffer.Bind<LightSources>(light_sources_buf.slot);
			p->DrawIndexed();
		}

		bsp_defs2d.clear();
		is_lightmap = false;
		light_source_index = 0;
	}
	else {

		renderer->GetContext()->PSSetSamplers(0, 1, &samplers[1]);
		renderer->GetContext()->RSSetViewports(1, &viewports[1]);

		if (renderer->index == 0) {
			ID3D11RenderTargetView* render_targets[8] = {
				renderer->render_target_views[EffectsRTV::SCENE],
				renderer->render_target_views[EffectsRTV::MASK],
				renderer->render_target_views[EffectsRTV::VELOSITY],
				renderer->render_target_views[EffectsRTV::LIGHTMAP],
				renderer->render_target_views[EffectsRTV::SCENE_COLOR],
				renderer->render_target_views[EffectsRTV::POSITIONS],
				renderer->render_target_views[EffectsRTV::NORMALS],
				renderer->render_target_views[EffectsRTV::ALBEDO],
			};

			renderer->GetContext()->OMSetRenderTargets(
				8u,
				render_targets,
				renderer->GetDepthStencilView(renderer->index)
			);
		}
		else {
			ID3D11RenderTargetView* render_targets[8] = {
				renderer->render_target_views[EffectsRTV::SCENE],
				renderer->render_target_views[EffectsRTV::MASK],
				renderer->render_target_views[EffectsRTV::VELOSITY],
				renderer->render_target_views[EffectsRTV::LIGHTMAP],
				renderer->render_target_views[EffectsRTV::SCENE_COLOR],
				renderer->render_target_views[EffectsRTV::POSITIONS],
				renderer->render_target_views[EffectsRTV::NORMALS],
				renderer->render_target_views[EffectsRTV::ALBEDO],
			};

			renderer->GetContext()->OMSetRenderTargets(
				8u,
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
			if (poly.lightmap_index > 1024) {
				int index = poly.lightmap_index - 1024;
				renderer->GetContext()->PSSetShaderResources(positionTexture.slot, 1, &pos_srv[index]);
				renderer->GetContext()->PSSetShaderResources(normalTexture.slot, 1, &norm_srv[index]);
				renderer->GetContext()->PSSetShaderResources(albedoTexture.slot, 1, &alb_srv[index]);
				renderer->GetContext()->PSSetShaderResources(lightmapTexture.slot, 1, &lightmap_srv[index]);
			}
			//renderer->Bind(poly.lightmap_index, lightmapTexture.slot);

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
	if (!IsGarbagePolygon(polygon.texture_index))
		bsp_defs.push_back(polygon);
}

void BSPRenderer::Add2d(const BSPDefinitions& polygon)
{
	if (!IsGarbagePolygon(polygon.texture_index))
		bsp_defs2d.push_back(polygon);
}

void BSPRenderer::Add2d2(const BSPDefinitions& polygon)
{
	//if (!IsGarbagePolygon(polygon.texture_index))
		//bsp_defs2d2.push_back(polygon);
}

void BSPRenderer::ModelPSProvider::PatchPipelineState(PipelineState* state, int defines) {
	States* states = States::GetInstance();

	if (defines & BSP_TEX)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
		state->dss = states->depth_stencil_states.at(DepthStencilState::NEVER);
	}	
	if (defines & BSP_TEX_LIGHTMAP)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
		state->dss = states->depth_stencil_states.at(DepthStencilState::NEVER);
	}

	if (defines & BSP_TEX_WIREFRAME)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::WIREFRAME);
		state->dss = states->depth_stencil_states.at(DepthStencilState::NEVER);
	}

	if (defines & BSP_TEX_LIGHTMAPWIRE)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::WIREFRAME);
		state->dss = states->depth_stencil_states.at(DepthStencilState::NEVER);
	}

	if (defines & BSP_ALPHA)
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
	}

	if (defines & BSP_WATER)
	{
		state->bs = states->blend_states.at(BlendState::ALPHABS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
	}

	if (defines & BSP_LIGHTMAPPEDPOLY)
	{
		state->bs = states->blend_states.at(BlendState::NOBS);
		state->rs = states->rasterization_states.at(RasterizationState::CULL_NONE);
		state->dss = states->depth_stencil_states.at(DepthStencilState::LESS);
	}

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

void BSPRenderer::ClearLightMaps() {
	Renderer* renderer = Renderer::GetInstance();
	for (int i = 0; i < 32; ++i) {
		renderer->GetContext()->ClearRenderTargetView(pos_rtv[i], DirectX::Colors::Black);
		renderer->GetContext()->ClearRenderTargetView(norm_rtv[i], DirectX::Colors::Black);
		renderer->GetContext()->ClearRenderTargetView(alb_rtv[i], DirectX::Colors::Black);
		renderer->GetContext()->ClearRenderTargetView(lightmap_rtv[i], DirectX::Colors::Black);
	}
}

bool BSPRenderer::IsGarbagePolygon(int texture_index)
{
	Renderer* renderer = Renderer::GetInstance();
	return ((texture_index == renderer->garbage_textures[0])
		|| (texture_index == renderer->garbage_textures[1])
		|| (texture_index == renderer->garbage_textures[2]));
}

BSPRenderer::ModelPSProvider* BSPRenderer::GetPSProvider()
{
	return reinterpret_cast<ModelPSProvider*>(factory->GetStateProvider());
}

void BSPRenderer::BindNewFactory()
{
	if (factory != nullptr)
		delete factory;

	factory = factory_temp;
}