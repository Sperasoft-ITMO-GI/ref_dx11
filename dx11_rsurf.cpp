// GL_RSURF.C: surface-related refresh code
#include <assert.h>
#include <algorithm>

#include <dx11_local.h>

static vec3_t	modelorg;		// relative to viewpoint

msurface_t* r_alpha_surfaces;

#define DYNAMIC_LIGHT_WIDTH  128
#define DYNAMIC_LIGHT_HEIGHT 128

#define LIGHTMAP_BYTES 4

#define	BLOCK_WIDTH		128
#define	BLOCK_HEIGHT	128

#define	MAX_LIGHTMAPS	128

int		c_visible_lightmaps;
int		c_visible_textures;

#define GL_LIGHTMAP_FORMAT GL_RGBA

typedef struct
{
	int internal_format;
	int	current_lightmap_texture;

	msurface_t* lightmap_surfaces[MAX_LIGHTMAPS];

	int			allocated[BLOCK_WIDTH];

	// the lightmap texture data needs to be kept in
	// main memory so texsubimage can update properly
	byte		lightmap_buffer[4 * BLOCK_WIDTH * BLOCK_HEIGHT];
} gllightmapstate_t;

static gllightmapstate_t gl_lms;


static void		LM_InitBlock(void);
static void		LM_UploadBlock(qboolean dynamic);
static qboolean	LM_AllocBlock(int w, int h, int* x, int* y);

extern void R_SetCacheState(msurface_t* surf);
extern void R_BuildLightMap(msurface_t* surf, byte* dest, int stride);

float colorBuf[4] = {1.0f, 1.0f, 1.0f, 1.0f};
bool multiTexture = true;

/*
=============================================================

	BRUSH MODELS

=============================================================
*/

/*
===============
R_TextureAnimation

Returns the proper texture for a given time and base texture
===============
*/
image_t* R_TextureAnimation(mtexinfo_t* tex)
{
	int		c;

	if (!tex->next)
		return tex->image;

	c = currententity->frame % tex->numframes;
	while (c)
	{
		tex = tex->next;
		c--;
	}

	return tex->image;
}

/*
================
DrawGLPoly
================
*/
void DrawGLPoly(glpoly_t* p, int texNum, int defines)
{
	int		i;
	float* v;

	BSPVertex vert = {};
	std::vector<BSPVertex> vect;

	v = p->verts[0];
	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
	{
		//qglTexCoord2f(v[3], v[4]);
		//qglVertex3fv(v);

		vert.position.x = v[0];
		vert.position.y = v[1];
		vert.position.z = v[2];
		vert.texture_coord.x = v[3];
		vert.texture_coord.y = v[4];

		vect.push_back(vert);
	}

	std::vector<UINT> indexes;

	SmartTriangulation(&indexes, p->numverts, 0);

	MODEL cb;
	cb.mod = DirectX::XMMatrixIdentity();
	cb.color.x = colorBuf[0];
	cb.color.y = colorBuf[1];
	cb.color.z = colorBuf[2];
	cb.color.w = colorBuf[3];

	BSPDefinitions bspd{
		vect, indexes, cb, {}, defines, texNum, -1
	};

	bsp_renderer->Add(bspd);
}

//============
//PGM
/*
================
DrawGLFlowingPoly -- version of DrawGLPoly that handles scrolling texture
================
*/
void DrawGLFlowingPoly(msurface_t* fa, int texNum, int defines)
{
	int		i;
	float* v;
	glpoly_t* p;
	float	scroll;

	p = fa->polys;

	scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));
	if (scroll == 0.0)
		scroll = -64.0;

	//qglBegin(GL_POLYGON);
	printf("DrawGLFlowingPoly found!!!\n");

	BSPVertex vert = {};
	std::vector<BSPVertex> vect;

	v = p->verts[0];
	for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
	{
		//qglTexCoord2f((v[3] + scroll), v[4]);
		//qglVertex3fv(v);

		vert.position.x = v[0];
		vert.position.y = v[1];
		vert.position.z = v[2];
		vert.texture_coord.x = v[3] + scroll;
		vert.texture_coord.y = v[4];

		vect.push_back(vert);
	}

	std::vector<UINT> indexes;

	SmartTriangulation(&indexes, p->numverts, 0);

	MODEL cb;
	cb.mod = DirectX::XMMatrixIdentity();
	cb.color.x = colorBuf[0];
	cb.color.y = colorBuf[1];
	cb.color.z = colorBuf[2];
	cb.color.w = colorBuf[3];

	BSPDefinitions bspd{
		vect, indexes, cb, {}, BSP_ALPHA, texNum, -1
	};

	bsp_renderer->Add(bspd);

	//qglEnd();
}
//PGM
//============


/*
** DrawGLPolyChain
*/
void DrawGLPolyChain(glpoly_t* p, float soffset, float toffset, int texNum)
{
	MODEL cb;
	cb.mod = DirectX::XMMatrixIdentity();
	cb.color.x = colorBuf[0];
	cb.color.y = colorBuf[1];
	cb.color.z = colorBuf[2];
	cb.color.w = colorBuf[3];

	if (soffset == 0 && toffset == 0)
	{
		for (; p != 0; p = p->chain)
		{
			float* v;
			int j;

			//qglBegin(GL_POLYGON);

			BSPVertex vert = {};
			std::vector<BSPVertex> vect;

			v = p->verts[0];
			for (j = 0; j < p->numverts; j++, v += VERTEXSIZE)
			{
				//qglTexCoord2f(v[5], v[6]);
				//qglVertex3fv(v);

				vert.position.x = v[0];
				vert.position.y = v[1];
				vert.position.z = v[2];
				vert.texture_coord.x = v[5];
				vert.texture_coord.y = v[6];

				vect.push_back(vert);
			}

			std::vector<UINT> indexes;

			SmartTriangulation(&indexes, p->numverts, 0);

			BSPDefinitions bspd{
				vect, indexes, cb, {}, BSP_ALPHA, texNum, -1
			};

			bsp_renderer->Add(bspd);

			//qglEnd();
		}
	}
	else
	{
		for (; p != 0; p = p->chain)
		{
			float* v;
			int j;

			//qglBegin(GL_POLYGON);

			BSPVertex vert = {};
			std::vector<BSPVertex> vect;

			v = p->verts[0];
			for (j = 0; j < p->numverts; j++, v += VERTEXSIZE)
			{
				//qglTexCoord2f(v[5] - soffset, v[6] - toffset);
				//qglVertex3fv(v);

				vert.position.x = v[0];
				vert.position.y = v[1];
				vert.position.z = v[2];
				vert.texture_coord.x = v[5] - soffset;
				vert.texture_coord.y = v[6] - toffset;

				vect.push_back(vert);
			}

			std::vector<UINT> indexes;

			SmartTriangulation(&indexes, p->numverts, 0);

			BSPDefinitions bspd{
				vect, indexes, cb, {}, BSP_ALPHA, texNum, -1
			};

			bsp_renderer->Add(bspd);

			//qglEnd();
		}
	}
}

/*
** R_BlendLightMaps
**
** This routine takes all the given light mapped surfaces in the world and
** blends them into the framebuffer.
*/
void R_BlendLightmaps(void)
{
	int			i;
	msurface_t* surf, * newdrawsurf = 0;

	// don't bother if we're set to fullbright
	if (r_fullbright->value)
		return;
	if (!r_worldmodel->lightdata)
		return;

	if (currentmodel == r_worldmodel)
		c_visible_lightmaps = 0;

	/*
	** render static lightmaps first
	*/
	for (i = 1; i < MAX_LIGHTMAPS; i++)
	{
		if (gl_lms.lightmap_surfaces[i])
		{
			if (currentmodel == r_worldmodel)
				c_visible_lightmaps++;
			//GL_Bind(dx11_state.lightmap_textures + i);

			for (surf = gl_lms.lightmap_surfaces[i]; surf != 0; surf = surf->lightmapchain)
			{
				if (surf->polys)
					DrawGLPolyChain(surf->polys, 0, 0, dx11_state.lightmap_textures + i);
			}
		}
	}

	/*
	** render dynamic lightmaps
	*/
	if (true/*gl_dynamic->value*/)
	{
		LM_InitBlock();

		//GL_Bind(dx11_state.lightmap_textures + 0);

		if (currentmodel == r_worldmodel)
			c_visible_lightmaps++;

		newdrawsurf = gl_lms.lightmap_surfaces[0];

		for (surf = gl_lms.lightmap_surfaces[0]; surf != 0; surf = surf->lightmapchain)
		{
			int		smax, tmax;
			byte* base;

			smax = (surf->extents[0] >> 4) + 1;
			tmax = (surf->extents[1] >> 4) + 1;

			if (LM_AllocBlock(smax, tmax, &surf->dlight_s, &surf->dlight_t))
			{
				base = gl_lms.lightmap_buffer;
				base += (surf->dlight_t * BLOCK_WIDTH + surf->dlight_s) * LIGHTMAP_BYTES;

				R_BuildLightMap(surf, base, BLOCK_WIDTH * LIGHTMAP_BYTES);
			}
			else
			{
				msurface_t* drawsurf;

				// upload what we have so far
				LM_UploadBlock(True);

				// draw all surfaces that use this lightmap
				for (drawsurf = newdrawsurf; drawsurf != surf; drawsurf = drawsurf->lightmapchain)
				{
					if (drawsurf->polys)
						DrawGLPolyChain(drawsurf->polys,
							(drawsurf->light_s - drawsurf->dlight_s) * (1.0 / 128.0),
							(drawsurf->light_t - drawsurf->dlight_t) * (1.0 / 128.0),
							dx11_state.lightmap_textures + 0);
				}

				newdrawsurf = drawsurf;

				// clear the block
				LM_InitBlock();

				// try uploading the block now
				if (!LM_AllocBlock(smax, tmax, &surf->dlight_s, &surf->dlight_t))
				{
					ri.Sys_Error(ERR_FATAL, "Consecutive calls to LM_AllocBlock(%d,%d) failed (dynamic)\n", smax, tmax);
				}

				base = gl_lms.lightmap_buffer;
				base += (surf->dlight_t * BLOCK_WIDTH + surf->dlight_s) * LIGHTMAP_BYTES;

				R_BuildLightMap(surf, base, BLOCK_WIDTH * LIGHTMAP_BYTES);
			}
		}

		/*
		** draw remainder of dynamic lightmaps that haven't been uploaded yet
		*/
		if (newdrawsurf)
			LM_UploadBlock(True);

		for (surf = newdrawsurf; surf != 0; surf = surf->lightmapchain)
		{
			if (surf->polys)
				DrawGLPolyChain(surf->polys,
					(surf->light_s - surf->dlight_s) * (1.0 / 128.0),
					(surf->light_t - surf->dlight_t) * (1.0 / 128.0),
					dx11_state.lightmap_textures + 0);
		}
	}

	/*
	** restore state
	*/
	//qglDisable(GL_BLEND);
	//qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//qglDepthMask(1);
}

/*
================
R_RenderBrushPoly
================
*/
void R_RenderBrushPoly(msurface_t* fa)
{
	int		 maps = 0;
	image_t* image;
	qboolean is_dynamic = False;

	c_brush_polys++;

	image = R_TextureAnimation(fa->texinfo);

	if (fa->flags & SURF_DRAWTURB)
	{
		//GL_Bind(image->texnum);

		// warp texture, no lightmaps
		//qglColor4f(dx11_state.inverse_intensity, dx11_state.inverse_intensity, dx11_state.inverse_intensity, 1.0F);
		colorBuf[0] = dx11_state.inverse_intensity;
		colorBuf[1] = dx11_state.inverse_intensity;
		colorBuf[2] = dx11_state.inverse_intensity;
		colorBuf[3] = 1.0f;
		EmitWaterPolys(fa);

		return;
	}
	else
	{
		//GL_Bind(image->texnum);

		//GL_TexEnv(GL_REPLACE);
	}

	//======
	//PGM
	if (fa->texinfo->flags & SURF_FLOWING)
		DrawGLFlowingPoly(fa, image->texnum, BSP_ALPHA);
	else
		DrawGLPoly(fa->polys, image->texnum, BSP_ALPHA);
	//PGM
	//======

	/*
	** check for lightmap modification
	*/
	for (maps = 0; maps < MAXLIGHTMAPS && fa->styles[maps] != 255; maps++)
	{
		if (r_newrefdef.lightstyles[fa->styles[maps]].white != fa->cached_light[maps])
			goto dynamic;
	}

	// dynamic this frame or dynamic previously
	if ((fa->dlightframe == r_framecount))
	{
	dynamic:
		if (/*gl_dynamic->value*/true)
		{
			if (!(fa->texinfo->flags & (SURF_SKY | SURF_TRANS33 | SURF_TRANS66 | SURF_WARP)))
			{
				is_dynamic = True;
			}
		}
	}

	if (is_dynamic)
	{
		if ((fa->styles[maps] >= 32 || fa->styles[maps] == 0) && (fa->dlightframe != r_framecount))
		{
			unsigned	temp[34 * 34];
			int			smax, tmax;

			smax = (fa->extents[0] >> 4) + 1;
			tmax = (fa->extents[1] >> 4) + 1;

			R_BuildLightMap(fa, (byte*)(void*)temp, smax * 4);
			R_SetCacheState(fa);

			/*GL_Bind(dx11_state.lightmap_textures + fa->lightmaptexturenum);
			qglTexSubImage2D(GL_TEXTURE_2D, 0,
				fa->light_s, fa->light_t,
				smax, tmax,
				GL_LIGHTMAP_FORMAT,
				GL_UNSIGNED_BYTE, temp);*/

			renderer->UpdateTextureInSRV(smax, tmax, fa->light_s, fa->light_t, 32,
				(unsigned char*)temp, dx11_state.lightmap_textures + fa->lightmaptexturenum);

			fa->lightmapchain = gl_lms.lightmap_surfaces[fa->lightmaptexturenum];
			gl_lms.lightmap_surfaces[fa->lightmaptexturenum] = fa;
		}
		else
		{
			fa->lightmapchain = gl_lms.lightmap_surfaces[0];
			gl_lms.lightmap_surfaces[0] = fa;
		}
	}
	else
	{
		fa->lightmapchain = gl_lms.lightmap_surfaces[fa->lightmaptexturenum];
		gl_lms.lightmap_surfaces[fa->lightmaptexturenum] = fa;
	}
}

/*
================
R_DrawAlphaSurfaces

Draw water surfaces and windows.
The BSP tree is waled front to back, so unwinding the chain
of alpha_surfaces will draw back to front, giving proper ordering.
================
*/
void R_DrawAlphaSurfaces(void)
{
	msurface_t* s;
	float		intens;

	//
	// go back to the world matrix
	//
	//qglLoadMatrixf(r_world_matrix);

	//qglEnable(GL_BLEND);
	//GL_TexEnv(GL_MODULATE);

	// the textures are prescaled up for a better lighting range,
	// so scale it back down
	intens = dx11_state.inverse_intensity;

	for (s = r_alpha_surfaces; s; s = s->texturechain)
	{
		//GL_Bind(s->texinfo->image->texnum);
		c_brush_polys++;
		if (s->texinfo->flags & SURF_TRANS33)
		{
			//qglColor4f(intens, intens, intens, 0.33);
			colorBuf[0] = intens;
			colorBuf[1] = intens;
			colorBuf[2] = intens;
			colorBuf[3] = 0.33;
		}
		else if (s->texinfo->flags & SURF_TRANS66)
		{
			//qglColor4f(intens, intens, intens, 0.66);
			colorBuf[0] = intens;
			colorBuf[1] = intens;
			colorBuf[2] = intens;
			colorBuf[3] = 0.66f;
		}
		else
		{
			//qglColor4f(intens, intens, intens, 1);
			colorBuf[0] = intens;
			colorBuf[1] = intens;
			colorBuf[2] = intens;
			colorBuf[3] = 1.0f;
		}
		if (s->flags & SURF_DRAWTURB)
			EmitWaterPolys(s);
		else
			DrawGLPoly(s->polys, s->texinfo->image->texnum, BSP_ALPHA);
	}

	//qglColor4f(1, 1, 1, 1);
	colorBuf[0] = 1.0f;
	colorBuf[1] = 1.0f;
	colorBuf[2] = 1.0f;
	colorBuf[3] = 1.0f;

	r_alpha_surfaces = NULL;
}

void CalculateNormals(std::vector<BSPVertex>& vect) {
	std::vector<BSPVertex> tempv(vect);
	std::sort(tempv.begin(), tempv.end(), [](const BSPVertex& lhs, const BSPVertex& rhs) {
		return (lhs.position.y > rhs.position.y) && (lhs.position.x > rhs.position.x);
	});
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 p0 = tempv[0].position;
	DirectX::XMFLOAT3 p1 = tempv[1].position;
	DirectX::XMFLOAT3 p2 = tempv[2].position;
	DirectX::XMVECTOR u = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));
	DirectX::XMVECTOR vn = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p0));
	DirectX::XMVECTOR temp;
	temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(u, vn));
	if (DirectX::XMVectorGetX(DirectX::XMVector3Length(temp)) > 0.00001) {
		DirectX::XMStoreFloat3(&normal, temp);
		for (auto& vert : vect) {
			vert.normal = normal;
		}
	}
	else if (vect.size() > 6) {
		p1 = tempv[4].position;
		p2 = tempv[5].position;
		u = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));
		vn = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p0));
		temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(u, vn));
		DirectX::XMStoreFloat3(&normal, temp);
		for (auto& vert : vect) {
			vert.normal = normal;
		}
	}
	else {
		p2 = tempv[3].position;
		u = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));
		vn = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p0));
		temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(u, vn));
		DirectX::XMStoreFloat3(&normal, temp);
		for (auto& vert : vect) {
			vert.normal = normal;
		}
	}


}

static void GL_RenderLightmappedPoly(msurface_t* surf, MODEL* cb)
{
	int		i, nv = surf->polys->numverts;
	int		map;
	float* v;
	image_t* image = R_TextureAnimation(surf->texinfo);
	qboolean is_dynamic = False;
	unsigned lmtex = surf->lightmaptexturenum;
	glpoly_t* p;

	for (map = 0; map < MAXLIGHTMAPS && surf->styles[map] != 255; map++)
	{
		if (r_newrefdef.lightstyles[surf->styles[map]].white != surf->cached_light[map])
			goto dynamic;
	}

	// dynamic this frame or dynamic previously
	if ((surf->dlightframe == r_framecount))
	{
	dynamic:
		if (true/*gl_dynamic->value*/)
		{
			if (!(surf->texinfo->flags & (SURF_SKY | SURF_TRANS33 | SURF_TRANS66 | SURF_WARP)))
			{
				is_dynamic = True;
			}
		}
	}

	if (is_dynamic)
	{
		unsigned	temp[128 * 128];
		int			smax, tmax;

		if ((surf->styles[map] >= 32 || surf->styles[map] == 0) && (surf->dlightframe != r_framecount))
		{
			smax = (surf->extents[0] >> 4) + 1;
			tmax = (surf->extents[1] >> 4) + 1;

			R_BuildLightMap(surf, (byte*)(void*)temp, smax * 4);
			R_SetCacheState(surf);

			lmtex = surf->lightmaptexturenum;

			//renderer->UpdateTextureInSRV(smax, tmax, surf->light_s, surf->light_t, 32,
			//	(unsigned char*)temp, dx11_state.lightmap_textures + surf->lightmaptexturenum);

			//if (cs_lm->value) {
			//	cs->Bind();
			//	renderer->GetContext()->CSSetUnorderedAccessViews(0, 1, &renderer->uav, nullptr);
			//	renderer->GetContext()->Dispatch(4, 4, 1);
			//	ID3D11UnorderedAccessView* UAV_NULL = NULL;
			//	renderer->GetContext()->CSSetUnorderedAccessViews(0, 1, &UAV_NULL, 0);
			//}

		}
		else
		{
			smax = (surf->extents[0] >> 4) + 1;
			tmax = (surf->extents[1] >> 4) + 1;

			R_BuildLightMap(surf, (byte*)(void*)temp, smax * 4);

			lmtex = 0;

			//renderer->UpdateTextureInSRV(smax, tmax, surf->light_s, surf->light_t, 32,
			//	(unsigned char*)temp, dx11_state.lightmap_textures + 0);

		}


		//if (cs_lm->value) {
		//	cs->Bind();
		//	renderer->GetContext()->CSSetUnorderedAccessViews(0, 1, &renderer->uav, nullptr);
		//	renderer->GetContext()->Dispatch(16, 16, 1);
		//	ID3D11UnorderedAccessView* UAV_NULL = NULL;
		//	renderer->GetContext()->CSSetUnorderedAccessViews(0, 1, &UAV_NULL, 0);
		//}



		c_brush_polys++;

		//GL_MBind(GL_TEXTURE0_SGIS, image->texnum);
		//GL_MBind(GL_TEXTURE1_SGIS, dx11_state.lightmap_textures + lmtex);

		//==========
		//PGM
		if (surf->texinfo->flags & SURF_FLOWING)
		{
			float scroll;

			scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));
			if (scroll == 0.0)
				scroll = -64.0;

			for (p = surf->polys; p; p = p->chain)
			{
				BSPVertex vert = {};
				std::vector<BSPVertex> vect;

				v = p->verts[0];
				//qglBegin(GL_POLYGON);
				for (i = 0; i < nv; i++, v += VERTEXSIZE)
				{
					//qglMTexCoord2fSGIS(GL_TEXTURE0_SGIS, (v[3] + scroll), v[4]);
					//qglMTexCoord2fSGIS(GL_TEXTURE1_SGIS, v[5], v[6]);
					//qglVertex3fv(v);

					vert.position.x = v[0];
					vert.position.y = v[1];
					vert.position.z = v[2];
					vert.texture_coord.x = v[3] + scroll;
					vert.texture_coord.y = v[4];
					vert.lightmap_texture_coord.x = v[5];
					vert.lightmap_texture_coord.y = v[6];

					vect.push_back(vert);
				}

				

				std::vector<UINT> indexes;

				SmartTriangulation(&indexes, nv, 0);

				cb->color.x = colorBuf[0];
				cb->color.y = colorBuf[1];
				cb->color.z = colorBuf[2];
				cb->color.w = colorBuf[3];

				// BSP_LIGHTMAPPEDPOLY

				mbuffer.orthogonal = DirectX::XMMatrixTranspose(
					DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 1.0f,
						1.0f, 0.0f, 0.0f, 1000.0f));
				//cbBuffer.Update(mbuffer);
				//cbBuffer.Bind<MatrixBuffer>(buffer.slot);

				if (bsp_renderer->is_first) {
					CalculateNormals(vect);
					BSPDefinitions bspd2{
						vect, indexes, *cb, mbuffer, BSP_TEX, image->texnum, lmtex
					};
					bsp_renderer->Add2d(bspd2);

					bspd2 = BSPDefinitions{
						vect, indexes, *cb, mbuffer, BSP_TEX_WIREFRAME, image->texnum, (int)lmtex
					};
					bsp_renderer->Add2d(bspd2);

					if (image->texnum == renderer->lamp_indexes[0] ||
						image->texnum == renderer->lamp_indexes[1] ||
						image->texnum == renderer->lamp_indexes[2] ||
						image->texnum == renderer->lamp_indexes[3]) {

						float origin[3] = { 0,  0,  0 };
						float max[3] = { -10000,  -10000,  -10000 };
						float min[3] = { 10000,  10000,  10000 };
						for (int i = 0; i < vect.size(); ++i) {
							if (vect[i].position.x > max[0]) {
								max[0] = vect[i].position.x;
							}
							if (vect[i].position.y > max[1]) {
								max[1] = vect[i].position.y;
							}
							if (vect[i].position.z > max[2]) {
								max[2] = vect[i].position.z;
							}
							if (vect[i].position.x < min[0]) {
								min[0] = vect[i].position.x;
							}
							if (vect[i].position.y < min[1]) {
								min[1] = vect[i].position.y;
							}
							if (vect[i].position.z < min[2]) {
								min[2] = vect[i].position.z;
							}
						}

						origin[0] = max[0] - (max[0] - min[0]) * 0.5 - vect[0].normal.x * 20;
						origin[1] = max[1] - (max[1] - min[1]) * 0.5 - vect[0].normal.y * 20;
						origin[2] = max[2] - (max[2] - min[2]) * 0.5 - vect[0].normal.z * 20;

						DirectX::XMFLOAT3 point(origin);
						bsp_renderer->light_sources[bsp_renderer->light_source_index++] = point;

						std::vector<UtilsVertex> uvert{
							{{origin[0] - 10, origin[1], origin[2]}, {0.0f, 0.0f}},
							{{origin[0] + 10, origin[1], origin[2]}, {1.0f, 0.0f}},
							{{origin[0], origin[1] - 10, origin[2]}, {1.0f, 1.0f}},
							{{origin[0], origin[1] + 10, origin[2]}, {0.0f, 1.0f}},
							{{origin[0], origin[1], origin[2] - 10}, {0.0f, 1.0f}},
							{{origin[0], origin[1], origin[2] + 10}, {0.0f, 1.0f}},
						};

						std::vector<UINT> uind{
							2, 1, 0, 0, 3, 2
						};

						UtilsDefinitions udefs{
							uvert, uind, {}, UTILS_STATIC
						};
						utils_renderer->Add(udefs);

					}
				}
				else if (bsp_renderer->is_lightmap) {
					BSPDefinitions bspd2{
						vect, indexes, *cb, mbuffer, BSP_TEX_LIGHTMAP, image->texnum, lmtex
					};
					bsp_renderer->Add2d(bspd2);

					bspd2 = BSPDefinitions{
						vect, indexes, *cb, mbuffer, BSP_TEX_LIGHTMAPWIRE, image->texnum, (int)lmtex
					};
					bsp_renderer->Add2d(bspd2);
				}
				else {
					CalculateNormals(vect);
					BSPDefinitions bspd{
						vect, indexes, *cb, mbuffer, BSP_LIGHTMAPPEDPOLY, image->texnum, dx11_state.lightmap_textures + lmtex
					};

					bsp_renderer->Add(bspd);

						// Immediate draw dynamic lightmapped polygon
						// before dynamic texture will updated
					bsp_renderer->Render();
				}

				//qglEnd();
			}
		}
		else
		{
			for (p = surf->polys; p; p = p->chain)
			{
				BSPVertex vert = {};
				std::vector<BSPVertex> vect;

				v = p->verts[0];
				//qglBegin(GL_POLYGON);
				for (i = 0; i < nv; i++, v += VERTEXSIZE)
				{
					//qglMTexCoord2fSGIS(GL_TEXTURE0_SGIS, v[3], v[4]);
					//qglMTexCoord2fSGIS(GL_TEXTURE1_SGIS, v[5], v[6]);
					//qglVertex3fv(v);

					//float soffset = (surf->light_s - surf->dlight_s) * (1.0 / 128.0);
					//float toffset = (surf->light_t - surf->dlight_t) * (1.0 / 128.0);

					vert.position.x = v[0];
					vert.position.y = v[1];
					vert.position.z = v[2];
					vert.texture_coord.x = v[3];
					vert.texture_coord.y = v[4];
					vert.lightmap_texture_coord.x = v[5];
					vert.lightmap_texture_coord.y = v[6];

					vect.push_back(vert);
				}

				

				std::vector<UINT> indexes;

				SmartTriangulation(&indexes, nv, 0);

				cb->color.x = colorBuf[0];
				cb->color.y = colorBuf[1];
				cb->color.z = colorBuf[2];
				cb->color.w = colorBuf[3];


				mbuffer.orthogonal = DirectX::XMMatrixTranspose(
					DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 1.0f,
						1.0f, 0.0f, 0.0f, 1000.0f));

				if (bsp_renderer->is_first) {
					CalculateNormals(vect);
					BSPDefinitions bspd2{
						vect, indexes, *cb, mbuffer, BSP_TEX, image->texnum, lmtex
					};
					bsp_renderer->Add2d(bspd2);

					bspd2 = BSPDefinitions{
						vect, indexes, *cb, mbuffer, BSP_TEX_WIREFRAME, image->texnum, (int)lmtex
					};
					bsp_renderer->Add2d(bspd2);

					if (image->texnum == renderer->lamp_indexes[0] ||
						image->texnum == renderer->lamp_indexes[1] ||
						image->texnum == renderer->lamp_indexes[2] ||
						image->texnum == renderer->lamp_indexes[3]) {

						float origin[3] = { 0,  0,  0 };
						float max[3] = { -10000,  -10000,  -10000 };
						float min[3] = { 10000,  10000,  10000 };
						for (int i = 0; i < vect.size(); ++i) {
							if (vect[i].position.x > max[0]) {
								max[0] = vect[i].position.x;
							}
							if (vect[i].position.y > max[1]) {
								max[1] = vect[i].position.y;
							}
							if (vect[i].position.z > max[2]) {
								max[2] = vect[i].position.z;
							}
							if (vect[i].position.x < min[0]) {
								min[0] = vect[i].position.x;
							}
							if (vect[i].position.y < min[1]) {
								min[1] = vect[i].position.y;
							}
							if (vect[i].position.z < min[2]) {
								min[2] = vect[i].position.z;
							}
						}

						origin[0] = max[0] - (max[0] - min[0]) * 0.5 - vect[0].normal.x * 20;
						origin[1] = max[1] - (max[1] - min[1]) * 0.5 - vect[0].normal.y * 20;
						origin[2] = max[2] - (max[2] - min[2]) * 0.5 - vect[0].normal.z * 20;

						DirectX::XMFLOAT3 point(origin);
						bsp_renderer->light_sources[bsp_renderer->light_source_index++] = point;

						std::vector<UtilsVertex> uvert{
							{{origin[0] - 10, origin[1], origin[2]}, {0.0f, 0.0f}},
							{{origin[0] + 10, origin[1], origin[2]}, {1.0f, 0.0f}},
							{{origin[0], origin[1] - 10, origin[2]}, {1.0f, 1.0f}},
							{{origin[0], origin[1] + 10, origin[2]}, {0.0f, 1.0f}},
							{{origin[0], origin[1], origin[2] - 10}, {0.0f, 1.0f}},
							{{origin[0], origin[1], origin[2] + 10}, {0.0f, 1.0f}},
						};

						std::vector<UINT> uind{
							2, 1, 0, 0, 3, 2
						};

						UtilsDefinitions udefs{
							uvert, uind, {}, UTILS_STATIC
						};
						utils_renderer->Add(udefs);

					}
				}
				else if (bsp_renderer->is_lightmap) {
					BSPDefinitions bspd2{
						vect, indexes, *cb, mbuffer, BSP_TEX_LIGHTMAP, image->texnum, lmtex
					};
					bsp_renderer->Add2d(bspd2);
					bspd2 = BSPDefinitions{
						vect, indexes, *cb, mbuffer, BSP_TEX_LIGHTMAPWIRE, image->texnum, (int)lmtex
					};
					bsp_renderer->Add2d(bspd2);
				}
				else {
					CalculateNormals(vect);
					BSPDefinitions bspd{
						vect, indexes, *cb, mbuffer, BSP_LIGHTMAPPEDPOLY, image->texnum, dx11_state.lightmap_textures + lmtex
					};

					bsp_renderer->Add(bspd);

						// Immediate draw dynamic lightmapped polygon
						// before dynamic texture will updated
					bsp_renderer->Render();
	
				}

				//qglEnd();
			}
		}
		//PGM
		//==========
	}
	else
	{

		c_brush_polys++;

		//GL_MBind(GL_TEXTURE0_SGIS, image->texnum);
		//GL_MBind(GL_TEXTURE1_SGIS, dx11_state.lightmap_textures + lmtex);

		//==========
		//PGM
		if (surf->texinfo->flags & SURF_FLOWING)
		{
			float scroll;

			scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));
			if (scroll == 0.0)
				scroll = -64.0;

			for (p = surf->polys; p; p = p->chain)
			{
				BSPVertex vert = {};
				std::vector<BSPVertex> vect;

				v = p->verts[0];
				//qglBegin(GL_POLYGON);
				for (i = 0; i < nv; i++, v += VERTEXSIZE)
				{
					//qglMTexCoord2fSGIS(GL_TEXTURE0_SGIS, (v[3] + scroll), v[4]);
					//qglMTexCoord2fSGIS(GL_TEXTURE1_SGIS, v[5], v[6]);
					//qglVertex3fv(v);

					vert.position.x = v[0];
					vert.position.y = v[1];
					vert.position.z = v[2];
					vert.texture_coord.x = v[3] + scroll;
					vert.texture_coord.y = v[4];
					vert.lightmap_texture_coord.x = v[5];
					vert.lightmap_texture_coord.y = v[6];

					vect.push_back(vert);
				}

				

				std::vector<UINT> indexes;

				SmartTriangulation(&indexes, nv, 0);

				cb->color.x = colorBuf[0];
				cb->color.y = colorBuf[1];
				cb->color.z = colorBuf[2];
				cb->color.w = colorBuf[3];

	
				mbuffer.orthogonal = DirectX::XMMatrixTranspose(
					DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 1.0f,
						1.0f, 0.0f, 0.0f, 1000.0f));
				//cbBuffer.Update(mbuffer);
				//cbBuffer.Bind<MatrixBuffer>(buffer.slot);

				if (bsp_renderer->is_first) {
					CalculateNormals(vect);
					BSPDefinitions bspd2{
						vect, indexes, *cb, mbuffer, BSP_TEX, image->texnum, lmtex
					};
					bsp_renderer->Add2d(bspd2);

					bspd2 = BSPDefinitions{
						vect, indexes, *cb, mbuffer, BSP_TEX_WIREFRAME, image->texnum, (int)lmtex
					};
					bsp_renderer->Add2d(bspd2);

					if (image->texnum == renderer->lamp_indexes[0] ||
						image->texnum == renderer->lamp_indexes[1] ||
						image->texnum == renderer->lamp_indexes[2] ||
						image->texnum == renderer->lamp_indexes[3]) {
						float origin[3] = { 0,  0,  0 };
						float max[3] = { -10000,  -10000,  -10000 };
						float min[3] = { 10000,  10000,  10000 };
						for (int i = 0; i < vect.size(); ++i) {
							if (vect[i].position.x > max[0]) {
								max[0] = vect[i].position.x;
							}
							if (vect[i].position.y > max[1]) {
								max[1] = vect[i].position.y;
							}
							if (vect[i].position.z > max[2]) {
								max[2] = vect[i].position.z;
							}
							if (vect[i].position.x < min[0]) {
								min[0] = vect[i].position.x;
							}
							if (vect[i].position.y < min[1]) {
								min[1] = vect[i].position.y;
							}
							if (vect[i].position.z < min[2]) {
								min[2] = vect[i].position.z;
							}
						}

						origin[0] = max[0] - (max[0] - min[0]) * 0.5 - vect[0].normal.x * 20;
						origin[1] = max[1] - (max[1] - min[1]) * 0.5 - vect[0].normal.y * 20;
						origin[2] = max[2] - (max[2] - min[2]) * 0.5 - vect[0].normal.z * 20;

						DirectX::XMFLOAT3 point(origin);
						bsp_renderer->light_sources[bsp_renderer->light_source_index++] = point;

						std::vector<UtilsVertex> uvert{
							{{origin[0] - 10, origin[1], origin[2]}, {0.0f, 0.0f}},
							{{origin[0] + 10, origin[1], origin[2]}, {1.0f, 0.0f}},
							{{origin[0], origin[1] - 10, origin[2]}, {1.0f, 1.0f}},
							{{origin[0], origin[1] + 10, origin[2]}, {0.0f, 1.0f}},
							{{origin[0], origin[1], origin[2] - 10}, {0.0f, 1.0f}},
							{{origin[0], origin[1], origin[2] + 10}, {0.0f, 1.0f}},
						};

						std::vector<UINT> uind{
							2, 1, 0, 0, 3, 2
						};

						UtilsDefinitions udefs{
							uvert, uind, {}, UTILS_STATIC
						};
						utils_renderer->Add(udefs);

					}
				}
				else if (bsp_renderer->is_lightmap) {
					BSPDefinitions bspd2{
						vect, indexes, *cb, mbuffer, BSP_TEX_LIGHTMAP, image->texnum, lmtex
					};
					bsp_renderer->Add2d(bspd2);
					bspd2 = BSPDefinitions{
						vect, indexes, *cb, mbuffer, BSP_TEX_LIGHTMAPWIRE, image->texnum, (int)lmtex
					};
					bsp_renderer->Add2d(bspd2);
				}
				else {
					CalculateNormals(vect);
					BSPDefinitions bspd{
						vect, indexes, *cb, mbuffer, BSP_LIGHTMAPPEDPOLY, image->texnum, dx11_state.lightmap_textures + lmtex
					};

					bsp_renderer->Add(bspd);
				}

				//qglEnd();
			}
		}
		else
		{
			//PGM
			//==========
			for (p = surf->polys; p; p = p->chain)
			{
				BSPVertex vert = {};
				std::vector<BSPVertex> vect;

				v = p->verts[0];
				//qglBegin(GL_POLYGON);
				for (i = 0; i < nv; i++, v += VERTEXSIZE)
				{
					//qglMTexCoord2fSGIS(GL_TEXTURE0_SGIS, v[3], v[4]);
					//qglMTexCoord2fSGIS(GL_TEXTURE1_SGIS, v[5], v[6]);
					//qglVertex3fv(v);

					vert.position.x = v[0];
					vert.position.y = v[1];
					vert.position.z = v[2];
					vert.texture_coord.x = v[3];
					vert.texture_coord.y = v[4];
					vert.lightmap_texture_coord.x = v[5];
					vert.lightmap_texture_coord.y = v[6];

					vect.push_back(vert);
				}

				std::vector<UINT> indexes;

				SmartTriangulation(&indexes, nv, 0);

				cb->color.x = colorBuf[0];
				cb->color.y = colorBuf[1];
				cb->color.z = colorBuf[2];
				cb->color.w = colorBuf[3];


				mbuffer.orthogonal = DirectX::XMMatrixTranspose(
					DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 1.0f,
						1.0f, 0.0f,  0.0f, 1000.0f));
				//cbBuffer.Update(mbuffer);
				//cbBuffer.Bind<MatrixBuffer>(buffer.slot);
				
				if (bsp_renderer->is_first) {
					CalculateNormals(vect);

					BSPDefinitions bspd2{
						vect, indexes, *cb, mbuffer, BSP_TEX, image->texnum, lmtex
					};
					bsp_renderer->Add2d(bspd2);

					bspd2 = BSPDefinitions{
						vect, indexes, *cb, mbuffer, BSP_TEX_WIREFRAME, image->texnum, (int)lmtex
					};
					bsp_renderer->Add2d(bspd2);

					if (image->texnum == renderer->lamp_indexes[0] ||
						image->texnum == renderer->lamp_indexes[1] ||
						image->texnum == renderer->lamp_indexes[2] ||
						image->texnum == renderer->lamp_indexes[3]) {

						float origin[3] = { 0,  0,  0 };
						float max[3] = { -10000,  -10000,  -10000 };
						float min[3] = { 10000,  10000,  10000 };
						for (int i = 0; i < vect.size(); ++i) {
							if (vect[i].position.x > max[0]) {
								max[0] = vect[i].position.x;
							}
							if (vect[i].position.y > max[1]) {
								max[1] = vect[i].position.y;
							}
							if (vect[i].position.z > max[2]) {
								max[2] = vect[i].position.z;
							}
							if (vect[i].position.x < min[0]) {
								min[0] = vect[i].position.x;
							}
							if (vect[i].position.y < min[1]) {
								min[1] = vect[i].position.y;
							}
							if (vect[i].position.z < min[2]) {
								min[2] = vect[i].position.z;
							}
						}

						origin[0] = max[0] - (max[0] - min[0]) * 0.5 - vect[0].normal.x * 20;
						origin[1] = max[1] - (max[1] - min[1]) * 0.5 - vect[0].normal.y * 20;
						origin[2] = max[2] - (max[2] - min[2]) * 0.5 - vect[0].normal.z * 20;

						DirectX::XMFLOAT3 point(origin);
						bsp_renderer->light_sources[bsp_renderer->light_source_index++] = point;

						std::vector<UtilsVertex> uvert{
							{{origin[0] - 10, origin[1], origin[2]}, {0.0f, 0.0f}},
							{{origin[0] + 10, origin[1], origin[2]}, {1.0f, 0.0f}},
							{{origin[0], origin[1] - 10, origin[2]}, {1.0f, 1.0f}},
							{{origin[0], origin[1] + 10, origin[2]}, {0.0f, 1.0f}},
							{{origin[0], origin[1], origin[2] - 10}, {0.0f, 1.0f}},
							{{origin[0], origin[1], origin[2] + 10}, {0.0f, 1.0f}},
						};

						std::vector<UINT> uind{
							2, 1, 0, 0, 3, 2
						};

						UtilsDefinitions udefs{
							uvert, uind, {}, UTILS_STATIC
						};
						utils_renderer->Add(udefs);

					}

				}
				else if (bsp_renderer->is_lightmap) {
					BSPDefinitions bspd2{
						vect, indexes, *cb, mbuffer, BSP_TEX_LIGHTMAP, image->texnum, lmtex
					};
					bsp_renderer->Add2d(bspd2);
					bspd2 = BSPDefinitions{
						vect, indexes, *cb, mbuffer, BSP_TEX_LIGHTMAPWIRE, image->texnum, (int)lmtex
					};
					bsp_renderer->Add2d(bspd2);
				}
				else {
					CalculateNormals(vect);
					BSPDefinitions bspd{
						vect, indexes, *cb, mbuffer, BSP_LIGHTMAPPEDPOLY, image->texnum, dx11_state.lightmap_textures + lmtex
					};

					bsp_renderer->Add(bspd);
				}


				

				//qglEnd();
			}
			//==========
			//PGM
		}
		//PGM
		//==========
	}
}

/*
================
DrawTextureChains
================
*/
void DrawTextureChains(void)
{
	int		i;
	msurface_t* s;
	image_t* image;

	c_visible_textures = 0;

	if (!multiTexture)
	{
		for (i = 0, image = dxtextures; i < numdxtextures; i++, image++)
		{
			if (!image->registration_sequence)
				continue;
			s = image->texturechain;
			if (!s)
				continue;
			c_visible_textures++;

			for (; s; s = s->texturechain)
				R_RenderBrushPoly(s);

			image->texturechain = NULL;
		}
	}
	else
	{
		for (i = 0, image = dxtextures; i < numdxtextures; i++, image++)
		{
			if (!image->registration_sequence)
				continue;
			if (!image->texturechain)
				continue;
			c_visible_textures++;

			for (s = image->texturechain; s; s = s->texturechain)
			{
				if (!(s->flags & SURF_DRAWTURB))
					R_RenderBrushPoly(s);
			}
		}

		for (i = 0, image = dxtextures; i < numdxtextures; i++, image++)
		{
			if (!image->registration_sequence)
				continue;
			s = image->texturechain;
			if (!s)
				continue;

			for (; s; s = s->texturechain)
			{
				if (s->flags & SURF_DRAWTURB)
					R_RenderBrushPoly(s);
			}

			image->texturechain = NULL;
		}
	}
}

/*
=================
R_DrawInlineBModel
=================
*/
void R_DrawInlineBModel(MODEL* cb)
{
	int			i, k;
	cplane_t* pplane;
	float		dot;
	msurface_t* psurf;
	dlight_t* lt;

	// в lt есть ориджин динамического источника света, его нужно визуализировать

	// calculate dynamic lighting for bmodel
	if (true/*!gl_flashblend->value*/)
	{
		lt = r_newrefdef.dlights;

		for (k = 0; k < r_newrefdef.num_dlights; k++, lt++)
		{
			R_MarkLights(lt, 1 << k, currentmodel->nodes + currentmodel->firstnode);
		}
	}

	psurf = &currentmodel->surfaces[currentmodel->firstmodelsurface];

	// if translucent
	if (currententity->flags & RF_TRANSLUCENT)
	{
		//qglColor4f(1, 1, 1, 0.25);
		colorBuf[0] = 1.0f;
		colorBuf[1] = 1.0f;
		colorBuf[2] = 1.0f;
		colorBuf[3] = 0.25f;
	}

	//
	// draw texture
	//
	for (i = 0; i < currentmodel->nummodelsurfaces; i++, psurf++)
	{
		// find which side of the node we are on
		pplane = psurf->plane;

		dot = DotProduct(modelorg, pplane->normal) - pplane->dist;

		// draw the polygon
		if (((psurf->flags & SURF_PLANEBACK) && (dot < -BACKFACE_EPSILON)) ||
			(!(psurf->flags & SURF_PLANEBACK) && (dot > BACKFACE_EPSILON)))
		{
			if (psurf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66))
			{	// add to the translucent chain
				psurf->texturechain = r_alpha_surfaces;
				r_alpha_surfaces = psurf;
			}
			else if (multiTexture && !(psurf->flags & SURF_DRAWTURB))
			{
				GL_RenderLightmappedPoly(psurf, cb);
			}
			else
			{
				//R_RenderBrushPoly(psurf);
			}
		}
	}

	colorBuf[0] = 1.0f;
	colorBuf[1] = 1.0f;
	colorBuf[2] = 1.0f;
	colorBuf[3] = 1.0f;
}

/*
=================
R_DrawBrushModel
=================
*/
void R_DrawBrushModel(entity_t* e)
{
	vec3_t		mins, maxs;
	int			i;
	qboolean	rotated;

	if (currentmodel->nummodelsurfaces == 0)
		return;

	currententity = e;
	dx11_state.currenttextures[0] = dx11_state.currenttextures[1] = -1;

	if (e->angles[0] || e->angles[1] || e->angles[2])
	{
		rotated = True;
		for (i = 0; i < 3; i++)
		{
			mins[i] = e->origin[i] - currentmodel->radius;
			maxs[i] = e->origin[i] + currentmodel->radius;
		}
	}
	else
	{
		rotated = False;
		VectorAdd(e->origin, currentmodel->mins, mins);
		VectorAdd(e->origin, currentmodel->maxs, maxs);
	}

	if (R_CullBox(mins, maxs))
		return;

	//qglColor3f(1, 1, 1);
	colorBuf[0] = 1.0f;
	colorBuf[1] = 1.0f;
	colorBuf[2] = 1.0f;
	colorBuf[3] = 1.0f;
	memset(gl_lms.lightmap_surfaces, 0, sizeof(gl_lms.lightmap_surfaces));

	VectorSubtract(r_newrefdef.vieworg, e->origin, modelorg);
	if (rotated)
	{
		vec3_t	temp;
		vec3_t	forward, right, up;

		VectorCopy(modelorg, temp);
		AngleVectors(e->angles, forward, right, up);
		modelorg[0] = DotProduct(temp, forward);
		modelorg[1] = -DotProduct(temp, right);
		modelorg[2] = DotProduct(temp, up);
	}

	//qglPushMatrix();
	//auto saveMatrix = renderer->GetModelView();

	MODEL cb;
	cb.mod = DirectX::XMMatrixIdentity();

	e->angles[0] = -e->angles[0];	// stupid quake bug
	e->angles[2] = -e->angles[2];	// stupid quake bug
	//renderer->SetModelViewMatrix(R_RotateForEntity(e) * renderer->GetModelView());
	cb.mod = R_RotateForEntity(e);
	e->angles[0] = -e->angles[0];	// stupid quake bug
	e->angles[2] = -e->angles[2];	// stupid quake bug

	//GL_EnableMultitexture(True);
	//GL_SelectTexture(GL_TEXTURE0_SGIS);
	//GL_TexEnv(GL_REPLACE);
	//GL_SelectTexture(GL_TEXTURE1_SGIS);
	//GL_TexEnv(GL_MODULATE);

	R_DrawInlineBModel(&cb);
	//GL_EnableMultitexture(False);

	//qglPopMatrix();
	//renderer->SetModelViewMatrix(saveMatrix);
}

/*
=============================================================

	WORLD MODEL

=============================================================
*/

/*
================
R_RecursiveWorldNode
================
*/
int maxlmtex = -1;
void R_RecursiveWorldNode(mnode_t* node) {
	int			c, side, sidebit;
	cplane_t* plane;
	msurface_t* surf, ** mark;
	mleaf_t* pleaf;
	float		dot;
	image_t* image;

	if (node->contents == CONTENTS_SOLID)
		return;		// solid

	if (node->visframe != r_visframecount)
		return;
	if (R_CullBox(node->minmaxs, node->minmaxs + 3))
		return;



	// if a leaf node, draw stuff
	if (node->contents != -1)
	{
		pleaf = (mleaf_t*)node;

		// check for door connected areas
		if (r_newrefdef.areabits)
		{
			if (!(r_newrefdef.areabits[pleaf->area >> 3] & (1 << (pleaf->area & 7))))
				return;		// not visible
		}

		mark = pleaf->firstmarksurface;
		c = pleaf->nummarksurfaces;

		if (c)
		{
			do
			{
				(*mark)->visframe = r_framecount;
				mark++;
			} while (--c);
		}

		return;
	}

	// node is just a decision point, so go down the apropriate sides

	// find which side of the node we are on
	plane = node->plane;

	switch (plane->type)
	{
	case PLANE_X:
		dot = modelorg[0] - plane->dist;
		break;
	case PLANE_Y:
		dot = modelorg[1] - plane->dist;
		break;
	case PLANE_Z:
		dot = modelorg[2] - plane->dist;
		break;
	default:
		dot = DotProduct(modelorg, plane->normal) - plane->dist;
		break;
	}

	if (dot >= 0)
	{
		side = 0;
		sidebit = 0;
	}
	else
	{
		side = 1;
		sidebit = SURF_PLANEBACK;
	}

	// recurse down the children, front side first
	R_RecursiveWorldNode(node->children[side]);

	// draw stuff
	for (c = node->numsurfaces, surf = r_worldmodel->surfaces + node->firstsurface; c; c--, surf++)
	{
		if (surf->visframe != r_framecount)
			continue;

		if ((surf->flags & SURF_PLANEBACK) != sidebit)
			continue;		// wrong side

		if (surf->texinfo->flags & SURF_SKY)
		{	// just adds to visible sky bounds
			R_AddSkySurface(surf);
		}
		else if (surf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66))
		{	// add to the translucent chain
			surf->texturechain = r_alpha_surfaces;
			r_alpha_surfaces = surf;
		}
		else
		{
			if (multiTexture && !(surf->flags & SURF_DRAWTURB))
			{
				MODEL cb;
				cb.mod = DirectX::XMMatrixIdentity();
				GL_RenderLightmappedPoly(surf, &cb);
			}
			else
			{
				// the polygon is visible, so add it to the texture
				// sorted chain
				// FIXME: this is a hack for animation
				image = R_TextureAnimation(surf->texinfo);
				surf->texturechain = image->texturechain;
				image->texturechain = surf;
			}
		}
	}

	// recurse down the back side
	R_RecursiveWorldNode(node->children[!side]);
}


/*
=============
R_DrawWorld
=============
*/
void R_DrawWorld(void)
{
	entity_t	ent;

	int c;
	mnode_t* node;
	msurface_s* surf;
	r_worldmodel->nodes;

	if (!r_drawworld->value)
		return;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	if (bsp_renderer->is_first) {
		bsp_renderer->ClearLightMaps();
		const auto pass_count = 2u;
		for (auto pass = 0; pass < pass_count; ++pass) {
			for (c = 0; c < r_worldmodel->numsurfaces; ++c)
			{
				surf = r_worldmodel->surfaces + c;

				// auto cycle the world frame for texture animation
				memset(&ent, 0, sizeof(ent));
				ent.frame = (int)(r_newrefdef.time * 2);
				currententity = &ent;

				MODEL cb;
				cb.mod = DirectX::XMMatrixIdentity();
				if (surf->lightmaptexturenum > maxlmtex)
					maxlmtex = surf->lightmaptexturenum;
				if (surf->lightmaptexturenum != 0)
					GL_RenderLightmappedPoly(surf, &cb);
			}
			bsp_renderer->Render();
			bsp_renderer->is_lightmap = true;
			//bsp_renderer->GetPSProvider()->is_first_lm_pass = false;
		}
	}
	//else {
	//	for (c = 0; c < r_worldmodel->numsurfaces; ++c)
	//	{
	//		surf = r_worldmodel->surfaces + c;

	//		// auto cycle the world frame for texture animation
	//		memset(&ent, 0, sizeof(ent));
	//		ent.frame = (int)(r_newrefdef.time * 2);
	//		currententity = &ent;

	//		MODEL cb;
	//		cb.mod = DirectX::XMMatrixIdentity();
	//		if (surf->lightmaptexturenum != 0)
	//			GL_RenderLightmappedPoly(surf, &cb);
	//	}
	//}
	



	currentmodel = r_worldmodel;

	VectorCopy(r_newrefdef.vieworg, modelorg);

	// auto cycle the world frame for texture animation
	memset(&ent, 0, sizeof(ent));
	ent.frame = (int)(r_newrefdef.time * 2);
	currententity = &ent;

	dx11_state.currenttextures[0] = dx11_state.currenttextures[1] = -1;

	//qglColor3f(1, 1, 1);
	colorBuf[0] = 1.0f;
	colorBuf[1] = 1.0f;
	colorBuf[2] = 1.0f;
	colorBuf[3] = 1.0f;
	memset(gl_lms.lightmap_surfaces, 0, sizeof(gl_lms.lightmap_surfaces));
	R_ClearSkyBox();

	if (multiTexture)
	{
		R_RecursiveWorldNode(r_worldmodel->nodes);
	}
	else
	{
		R_RecursiveWorldNode(r_worldmodel->nodes);
	}

	/*
	** theoretically nothing should happen in the next two functions
	** if multitexture is enabled
	*/
	// Happen, this is happen!
	DrawTextureChains();
	R_BlendLightmaps();

	R_DrawSkyBox();
}


/*
===============
R_MarkLeaves

Mark the leaves and nodes that are in the PVS for the current
cluster
===============
*/
void R_MarkLeaves(void)
{
	byte* vis;
	byte	fatvis[MAX_MAP_LEAFS / 8];
	mnode_t* node;
	int		i, c;
	mleaf_t* leaf;
	int		cluster;

	if (r_oldviewcluster == r_viewcluster && r_oldviewcluster2 == r_viewcluster2 && !r_novis->value && r_viewcluster != -1)
		return;

	// development aid to let you run around and see exactly where
	// the pvs ends
	/*if (gl_lockpvs->value)
		return;*/

	r_visframecount++;
	r_oldviewcluster = r_viewcluster;
	r_oldviewcluster2 = r_viewcluster2;

	if (r_novis->value || r_viewcluster == -1 || !r_worldmodel->vis)
	{
		// mark everything
		for (i = 0; i < r_worldmodel->numleafs; i++)
			r_worldmodel->leafs[i].visframe = r_visframecount;
		for (i = 0; i < r_worldmodel->numnodes; i++)
			r_worldmodel->nodes[i].visframe = r_visframecount;
		return;
	}

	vis = Mod_ClusterPVS(r_viewcluster, r_worldmodel);
	// may have to combine two clusters because of solid water boundaries
	if (r_viewcluster2 != r_viewcluster)
	{
		memcpy(fatvis, vis, (r_worldmodel->numleafs + 7) / 8);
		vis = Mod_ClusterPVS(r_viewcluster2, r_worldmodel);
		c = (r_worldmodel->numleafs + 31) / 32;
		for (i = 0; i < c; i++)
			((int*)fatvis)[i] |= ((int*)vis)[i];
		vis = fatvis;
	}

	for (i = 0, leaf = r_worldmodel->leafs; i < r_worldmodel->numleafs; i++, leaf++)
	{
		cluster = leaf->cluster;
		if (cluster == -1)
			continue;
		if (vis[cluster >> 3] & (1 << (cluster & 7)))
		{
			node = (mnode_t*)leaf;
			do
			{
				if (node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while (node);
		}
	}
}



/*
=============================================================================

  LIGHTMAP ALLOCATION

=============================================================================
*/

static void LM_InitBlock(void)
{
	memset(gl_lms.allocated, 0, sizeof(gl_lms.allocated));
}

static void LM_UploadBlock(qboolean dynamic)
{
	int texture;
	int height = 0;

	if (dynamic)
	{
		texture = 0;
	}
	else
	{
		texture = gl_lms.current_lightmap_texture;
	}

	//GL_Bind(dx11_state.lightmap_textures + texture);
	//qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (dynamic)
	{
		int i;

		for (i = 0; i < BLOCK_WIDTH; i++)
		{
			if (gl_lms.allocated[i] > height)
				height = gl_lms.allocated[i];
		}

		renderer->UpdateTextureInSRV(BLOCK_WIDTH, height, 0, 0, 32,
			gl_lms.lightmap_buffer, dx11_state.lightmap_textures + texture);
		/*qglTexSubImage2D(GL_TEXTURE_2D,
			0,
			0, 0,
			BLOCK_WIDTH, height,
			GL_LIGHTMAP_FORMAT,
			GL_UNSIGNED_BYTE,
			gl_lms.lightmap_buffer);*/
	}
	else
	{
		char name[30] = "lm_tex_";
		char value[10];
		_itoa(texture, value, 10);
		strcat(name, value);
		renderer->AddTexturetoSRV(name, BLOCK_WIDTH, BLOCK_HEIGHT, 32, 
			gl_lms.lightmap_buffer, dx11_state.lightmap_textures + texture, true);
		/*qglTexImage2D(GL_TEXTURE_2D,
			0,
			gl_lms.internal_format,
			BLOCK_WIDTH, BLOCK_HEIGHT,
			0,
			GL_LIGHTMAP_FORMAT,
			GL_UNSIGNED_BYTE,
			gl_lms.lightmap_buffer);*/
		if (++gl_lms.current_lightmap_texture == MAX_LIGHTMAPS)
			ri.Sys_Error(ERR_DROP, "LM_UploadBlock() - MAX_LIGHTMAPS exceeded\n");
	}
}

// returns a texture number and the position inside it
static qboolean LM_AllocBlock(int w, int h, int* x, int* y)
{
	int		i, j;
	int		best, best2;

	best = BLOCK_HEIGHT;

	for (i = 0; i < BLOCK_WIDTH - w; i++)
	{
		best2 = 0;

		for (j = 0; j < w; j++)
		{
			if (gl_lms.allocated[i + j] >= best)
				break;
			if (gl_lms.allocated[i + j] > best2)
				best2 = gl_lms.allocated[i + j];
		}
		if (j == w)
		{	// this is a valid spot
			*x = i;
			*y = best = best2;
		}
	}

	if (best + h > BLOCK_HEIGHT)
		return False;

	for (i = 0; i < w; i++)
		gl_lms.allocated[*x + i] = best + h;

	return True;
}

/*
================
GL_BuildPolygonFromSurface
================
*/
void GL_BuildPolygonFromSurface(msurface_t* fa)
{
	int			i, lindex, lnumverts;
	medge_t* pedges, * r_pedge;
	int			vertpage;
	float* vec;
	float		s, t;
	glpoly_t* poly;
	vec3_t		total;

	// reconstruct the polygon
	pedges = currentmodel->edges;
	lnumverts = fa->numedges;
	vertpage = 0;

	VectorClear(total);
	//
	// draw texture
	//
	poly = (glpoly_t*)Hunk_Alloc(sizeof(glpoly_t) + (lnumverts - 4) * VERTEXSIZE * sizeof(float));
	poly->next = fa->polys;
	poly->flags = fa->flags;
	fa->polys = poly;
	poly->numverts = lnumverts;

	for (i = 0; i < lnumverts; i++)
	{
		lindex = currentmodel->surfedges[fa->firstedge + i];

		if (lindex > 0)
		{
			r_pedge = &pedges[lindex];
			vec = currentmodel->vertexes[r_pedge->v[0]].position;
		}
		else
		{
			r_pedge = &pedges[-lindex];
			vec = currentmodel->vertexes[r_pedge->v[1]].position;
		}
		s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
		s /= fa->texinfo->image->width;

		t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
		t /= fa->texinfo->image->height;

		VectorAdd(total, vec, total);
		VectorCopy(vec, poly->verts[i]);
		poly->verts[i][3] = s;
		poly->verts[i][4] = t;

		//
		// lightmap texture coordinates
		//
		s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
		s -= fa->texturemins[0];
		s += fa->light_s * 16;
		s += 8;
		s /= BLOCK_WIDTH * 16; //fa->texinfo->texture->width;

		t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
		t -= fa->texturemins[1];
		t += fa->light_t * 16;
		t += 8;
		t /= BLOCK_HEIGHT * 16; //fa->texinfo->texture->height;

		poly->verts[i][5] = s;
		poly->verts[i][6] = t;
	}

	poly->numverts = lnumverts;

}

/*
========================
GL_CreateSurfaceLightmap
========================
*/
void GL_CreateSurfaceLightmap(msurface_t* surf)
{
	int		smax, tmax;
	byte* base;

	if (surf->flags & (SURF_DRAWSKY | SURF_DRAWTURB))
		return;

	smax = (surf->extents[0] >> 4) + 1;
	tmax = (surf->extents[1] >> 4) + 1;

	if (!LM_AllocBlock(smax, tmax, &surf->light_s, &surf->light_t))
	{
		LM_UploadBlock(False);
		LM_InitBlock();
		if (!LM_AllocBlock(smax, tmax, &surf->light_s, &surf->light_t))
		{
			ri.Sys_Error(ERR_FATAL, "Consecutive calls to LM_AllocBlock(%d,%d) failed\n", smax, tmax);
		}
	}

	surf->lightmaptexturenum = gl_lms.current_lightmap_texture;

	base = gl_lms.lightmap_buffer;
	base += (surf->light_t * BLOCK_WIDTH + surf->light_s) * LIGHTMAP_BYTES;

	R_SetCacheState(surf);
	R_BuildLightMap(surf, base, BLOCK_WIDTH * LIGHTMAP_BYTES);
}


/*
==================
GL_BeginBuildingLightmaps

==================
*/
void GL_BeginBuildingLightmaps(model_t* m)
{
	static lightstyle_t	lightstyles[MAX_LIGHTSTYLES];
	int				i;
	unsigned		dummy[128 * 128];

	memset(gl_lms.allocated, 0, sizeof(gl_lms.allocated));

	r_framecount = 1;		// no dlightcache

	//GL_EnableMultitexture(True);
	//GL_SelectTexture(GL_TEXTURE1_SGIS);

	/*
	** setup the base lightstyles so the lightmaps won't have to be regenerated
	** the first time they're seen
	*/
	for (i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		lightstyles[i].rgb[0] = 1;
		lightstyles[i].rgb[1] = 1;
		lightstyles[i].rgb[2] = 1;
		lightstyles[i].white = 3;
	}
	r_newrefdef.lightstyles = lightstyles;

	if (!dx11_state.lightmap_textures)
	{
		dx11_state.lightmap_textures = TEXNUM_LIGHTMAPS;
	}

	gl_lms.current_lightmap_texture = 1;

	/*
	** if mono lightmaps are enabled and we want to use alpha
	** blending (a,1-a) then we're likely running on a 3DLabs
	** Permedia2.  In a perfect world we'd use a GL_ALPHA lightmap
	** in order to conserve space and maximize bandwidth, however
	** this isn't a perfect world.
	**
	** So we have to use alpha lightmaps, but stored in GL_RGBA format,
	** which means we only get 1/16th the color resolution we should when
	** using alpha lightmaps.  If we find another board that supports
	** only alpha lightmaps but that can at least support the GL_ALPHA
	** format then we should change this code to use real alpha maps.
	*/
	/*if (toupper(gl_monolightmap->string[0]) == 'A')
	{
		gl_lms.internal_format = gl_tex_alpha_format;
	}*/
	/*
	** try to do hacked colored lighting with a blended texture
	*/
	/*else if (toupper(gl_monolightmap->string[0]) == 'C')
	{
		gl_lms.internal_format = gl_tex_alpha_format;
	}
	else if (toupper(gl_monolightmap->string[0]) == 'I')
	{
		gl_lms.internal_format = GL_INTENSITY8;
	}
	else if (toupper(gl_monolightmap->string[0]) == 'L')
	{
		gl_lms.internal_format = GL_LUMINANCE8;
	}
	else
	{
		gl_lms.internal_format = gl_tex_solid_format;
	}*/

	gl_lms.internal_format = 3;//gl_tex_solid_format;

	/*
	** initialize the dynamic lightmap texture
	*/
	//GL_Bind(dx11_state.lightmap_textures + 0);
	//qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/*qglTexImage2D(GL_TEXTURE_2D,
		0,
		gl_lms.internal_format,
		BLOCK_WIDTH, BLOCK_HEIGHT,
		0,
		GL_LIGHTMAP_FORMAT,
		GL_UNSIGNED_BYTE,
		dummy);*/

	char name[] = "lm_tex_0";

	renderer->AddTexturetoSRV(name, BLOCK_WIDTH, BLOCK_HEIGHT, 32,
		(unsigned char*)dummy, dx11_state.lightmap_textures + 0, true);
}

/*
=======================
GL_EndBuildingLightmaps
=======================
*/
void GL_EndBuildingLightmaps(void)
{
	LM_UploadBlock(False);
	//GL_EnableMultitexture(False);
}

