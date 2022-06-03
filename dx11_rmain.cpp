// dx11r_main.c

#include <dx11_local.h>

void R_Clear(void);

viddef_t	vid;

refimport_t	ri;

model_t* r_worldmodel;

float		gldepthmin, gldepthmax;

image_t* r_notexture;		// use for bad textures
image_t* r_particletexture;	// little dot for particles

entity_t* currententity;
model_t* currentmodel;

cplane_t	frustum[4];

int			r_visframecount;	// bumped when going to a new PVS
int			r_framecount;		// used for dlight push checking

int			c_brush_polys, c_alias_polys;

float		v_blend[4];			// final blending color

void GL_Strings_f(void);

//
// view origin
//
vec3_t	vup;
vec3_t	vpn;
vec3_t	vright;
vec3_t	r_origin;

float	r_world_matrix[16];
float	r_base_world_matrix[16];

//
// screen size info
//
refdef_t	r_newrefdef;

int		r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

cvar_t* r_norefresh;
cvar_t* r_drawentities;
cvar_t* r_drawworld;
cvar_t* r_speeds;
cvar_t* r_fullbright;
cvar_t* r_novis;
cvar_t* r_nocull;
cvar_t* r_lerpmodels;
cvar_t* r_lefthand;

cvar_t* r_lightlevel;	// FIXME: This is a HACK to get the client's light level

cvar_t* gl_skymip;
cvar_t* gl_picmip;


cvar_t* dx11_mode;
cvar_t* taa;
cvar_t* is_directional_light;
cvar_t* light_color_r;
cvar_t* light_color_g;
cvar_t* light_color_b;
cvar_t* light_color_a;
cvar_t* light_direction_x;
cvar_t* light_direction_y;
cvar_t* light_direction_z;
cvar_t* light_intensity;
cvar_t* light_sources;
cvar_t* cs_lm;
cvar_t* positions;
cvar_t* normals;
cvar_t* albedo;
cvar_t* lightmap_index;
cvar_t* single_point_light_x;
cvar_t* single_point_light_y;
cvar_t* single_point_light_z;

cvar_t* vid_fullscreen;
cvar_t* vid_gamma;
cvar_t* vid_ref;

CAMERA cam = {};
ConstantBuffer<CAMERA> cbCamera;
DirectionalLight directional_light;
ConstantBuffer<DirectionalLight> cbDirectionalLight;
MatrixBuffer mbuffer;
ConstantBuffer<MatrixBuffer> cbBuffer;

dx11config_t dx11_config;
dx11state_t  dx11_state;

Renderer* renderer = Renderer::GetInstance();
UIRenderer* ui_renderer =     new UIRenderer(); // Зачем указатель? Ответа на этот вопрос у меня нет...
BSPRenderer* bsp_renderer =   new BSPRenderer();
SkyRenderer* sky_renderer =   new SkyRenderer();
BeamRenderer* beam_renderer = new BeamRenderer();
ModRenderer* mod_renderer = new ModRenderer();
ParticlesRenderer* particles_renderer = new ParticlesRenderer();
EffectsRenderer* effects_renderer = new EffectsRenderer();
UtilsRenderer* utils_renderer = new UtilsRenderer();

States* States::states = nullptr;

bool is_first = true;

cvar_t* gl_round_down;

std::chrono::time_point<std::chrono::steady_clock> prev_time = std::chrono::steady_clock::now();
float total_time = 0;
unsigned int frame_count = 0;
float fps = 0;

ComputeShader* cs = new ComputeShader();

void CompileShaders()
{
	// to recompile just input "cl" in colsole
	
	// UI_Renderer
	ui_renderer->InitNewFactory(L"ref_dx11\\shaders\\UITexture.hlsl");
	if (!ui_renderer->CompileWithDefines(UI_COLORED))
		ui_renderer->ClearTempFactory();
	else if (!ui_renderer->CompileWithDefines(UI_TEXTURED))
		ui_renderer->ClearTempFactory();
	else
		ui_renderer->BindNewFactory();

	// BSP_Renderer
	bsp_renderer->InitNewFactory(L"ref_dx11\\shaders\\BSP.hlsl");
	if (!bsp_renderer->CompileWithDefines(BSP_ALPHA))
		bsp_renderer->ClearTempFactory();
	else if (!bsp_renderer->CompileWithDefines(BSP_LIGHTMAPPEDPOLY))
		bsp_renderer->ClearTempFactory();
	else if (!bsp_renderer->CompileWithDefines(BSP_WATER))
		bsp_renderer->ClearTempFactory();
	else if (!bsp_renderer->CompileWithDefines(BSP_TEX))
		bsp_renderer->ClearTempFactory();
	else if (!bsp_renderer->CompileWithDefines(BSP_TEX_WIREFRAME))
		bsp_renderer->ClearTempFactory();	
	else if (!bsp_renderer->CompileWithDefines(BSP_TEX_LIGHTMAP))
		bsp_renderer->ClearTempFactory();
	else if (!bsp_renderer->CompileWithDefines(BSP_TEX_LIGHTMAPWIRE))
		bsp_renderer->ClearTempFactory();
	else 
		bsp_renderer->BindNewFactory();
	bsp_renderer->is_first = true;
	bsp_renderer->is_lightmap = false;
	bsp_renderer->GetPSProvider()->is_first_lm_pass = true;
	bsp_renderer->ClearLightMaps();

	// SKY_Renderer
	sky_renderer->InitNewFactory(L"ref_dx11\\shaders\\Sky.hlsl");
	if (!sky_renderer->CompileWithDefines(SKY_DEFAULT))
		sky_renderer->ClearTempFactory();
	else
		sky_renderer->BindNewFactory();

	// Beam_Renderer
	beam_renderer->InitNewFactory(L"ref_dx11\\shaders\\Beam.hlsl");
	if (!beam_renderer->CompileWithDefines(BEAM_DEFAULT))
		beam_renderer->ClearTempFactory();
	else
		beam_renderer->BindNewFactory();

	// Mod_Renderer
	mod_renderer->InitNewFactory(L"ref_dx11\\shaders\\Model.hlsl");
	if (!mod_renderer->CompileWithDefines(MOD_ALPHA))
		mod_renderer->ClearTempFactory();
	else
		mod_renderer->BindNewFactory();

	// Particles_Renderer
	particles_renderer->InitNewFactory(L"ref_dx11\\shaders\\Particles.hlsl");
	if (!particles_renderer->CompileWithDefines(PARTICLES_DEFAULT))
		particles_renderer->ClearTempFactory();
	else
		particles_renderer->BindNewFactory();

	// Effects_Renderer
	effects_renderer->InitNewFactory(L"ref_dx11\\shaders\\Effects.hlsl");
	if (!effects_renderer->CompileWithDefines(EFFECTS_DEFAULT))
		effects_renderer->ClearTempFactory();
	else if (!effects_renderer->CompileWithDefines(EFFECTS_SCENE))
		effects_renderer->ClearTempFactory();
	else
		effects_renderer->BindNewFactory();	
	
	utils_renderer->InitNewFactory(L"ref_dx11\\shaders\\Utils.hlsl");
	if (!utils_renderer->CompileWithDefines(UTILS_STATIC))
		utils_renderer->ClearTempFactory();
	else
		utils_renderer->BindNewFactory();

	ID3DBlob* blob = CompileShader(L"ref_dx11\\shaders\\LightmapCS.hlsl", NULL, "CSMain", "cs_5_0");
	cs->~ComputeShader();
	if (blob != nullptr)
		cs->Create(blob);
}


// ========================================================================================================================

/*
=================
R_CullBox

Returns True if the box is completely outside the frustom
=================
*/
qboolean R_CullBox(vec3_t mins, vec3_t maxs)
{
	int		i;

	if (r_nocull->value)
		return False;

	for (i = 0; i < 4; i++)
		if (BOX_ON_PLANE_SIDE(mins, maxs, &frustum[i]) == 2)
			return True;
	return False;
}


DirectX::XMMATRIX R_RotateForEntity(entity_t* e)
{
	/*qglTranslatef(e->origin[0], e->origin[1], e->origin[2]);

	qglRotatef(e->angles[1], 0, 0, 1);
	qglRotatef(-e->angles[0], 0, 1, 0);
	qglRotatef(-e->angles[2], 1, 0, 0);*/

	using namespace DirectX;

	XMMATRIX mat =
		XMMatrixRotationAxis({ 1, 0, 0 }, XMConvertToRadians(-e->angles[2])) *
		XMMatrixRotationAxis({ 0, 1, 0 }, XMConvertToRadians(-e->angles[0])) *
		XMMatrixRotationAxis({ 0, 0, 1 }, XMConvertToRadians(e->angles[1])) *
		XMMatrixTranslation(e->origin[0], e->origin[1], e->origin[2]);

	return mat;
}

/*
=============================================================

  SPRITE MODELS

=============================================================
*/


/*
=================
R_DrawSpriteModel

=================
*/
void R_DrawSpriteModel(entity_t* e)
{
	float alpha = 1.0F;
	vec3_t	point;
	dsprframe_t* frame;
	float* up, * right;
	dsprite_t* psprite;

	// don't even bother culling, because it's just a single
	// polygon without a surface cache

	psprite = (dsprite_t*)currentmodel->extradata;

	e->frame %= psprite->numframes;

	frame = &psprite->frames[e->frame];

	{	// normal sprite
		up = vup;
		right = vright;
	}

	if (e->flags & RF_TRANSLUCENT)
		alpha = e->alpha;

	//qglColor4f(1, 1, 1, alpha);
	colorBuf[0] = 1.0f;
	colorBuf[1] = 1.0f;
	colorBuf[2] = 1.0f;
	colorBuf[3] = alpha;

	//GL_Bind(currentmodel->skins[e->frame]->texnum);

	//qglBegin(GL_QUADS);

	MODEL cb;
	cb.color.x = colorBuf[0];
	cb.color.y = colorBuf[1];
	cb.color.z = colorBuf[2];
	cb.color.w = colorBuf[3];

	std::vector<BSPVertex> vect;

	//qglTexCoord2f(0, 1);
	VectorMA(e->origin, -frame->origin_y, up, point);
	VectorMA(point, -frame->origin_x, right, point);
	//qglVertex3fv(point);
	vect.push_back({ {point[0], point[1], point[2]}, {}, {0, 1}, {0, 0} });

	//qglTexCoord2f(0, 0);
	VectorMA(e->origin, frame->height - frame->origin_y, up, point);
	VectorMA(point, -frame->origin_x, right, point);
	//qglVertex3fv(point);
	vect.push_back({ {point[0], point[1], point[2]}, {}, {0, 0}, {0, 0} });

	//qglTexCoord2f(1, 0);
	VectorMA(e->origin, frame->height - frame->origin_y, up, point);
	VectorMA(point, frame->width - frame->origin_x, right, point);
	//qglVertex3fv(point);
	vect.push_back({ {point[0], point[1], point[2]}, {}, {1, 0}, {0, 0} });

	//qglTexCoord2f(1, 1);
	VectorMA(e->origin, -frame->origin_y, up, point);
	VectorMA(point, frame->width - frame->origin_x, right, point);
	//qglVertex3fv(point);
	vect.push_back({ {point[0], point[1], point[2]}, {},  {1, 1}, {0, 0} });

	std::vector<UINT> indexes;

	indexes.push_back(0);
	indexes.push_back(2);
	indexes.push_back(1);

	indexes.push_back(0);
	indexes.push_back(3);
	indexes.push_back(2);

	BSPDefinitions bspd{
		vect, indexes, cb, {}, BSP_ALPHA, currentmodel->skins[e->frame]->texnum, -1
	};

	bsp_renderer->Add(bspd);

	//qglEnd();

	//qglColor4f(1, 1, 1, 1);
	colorBuf[0] = 1.0f;
	colorBuf[1] = 1.0f;
	colorBuf[2] = 1.0f;
	colorBuf[3] = 1.0f;
}

//==================================================================================

/*
=============
R_DrawNullModel
=============
*/
void R_DrawNullModel(void)
{
	/*vec3_t	shadelight;
	int		i;

	if (currententity->flags & RF_FULLBRIGHT)
		shadelight[0] = shadelight[1] = shadelight[2] = 1.0F;
	else
		R_LightPoint(currententity->origin, shadelight);

	qglPushMatrix();
	R_RotateForEntity(currententity);

	qglDisable(GL_TEXTURE_2D);
	qglColor3fv(shadelight);

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3f(0, 0, -16);
	for (i = 0; i <= 4; i++)
		qglVertex3f(16 * cos(i * M_PI / 2), 16 * sin(i * M_PI / 2), 0);
	qglEnd();

	qglBegin(GL_TRIANGLE_FAN);
	qglVertex3f(0, 0, 16);
	for (i = 4; i >= 0; i--)
		qglVertex3f(16 * cos(i * M_PI / 2), 16 * sin(i * M_PI / 2), 0);
	qglEnd();

	qglColor3f(1, 1, 1);
	qglPopMatrix();
	qglEnable(GL_TEXTURE_2D);*/
}

/*
=============
R_DrawEntitiesOnList
=============
*/
void R_DrawEntitiesOnList(void)
{
	int		i;

	if (!r_drawentities->value)
		return;

	// draw non-transparent first
	for (i = 0; i < r_newrefdef.num_entities; i++)
	{
		currententity = &r_newrefdef.entities[i];
		if (currententity->flags & RF_TRANSLUCENT)
			continue;	// solid

		if (currententity->flags & RF_BEAM)
		{
			R_DrawBeam(currententity);
		}
		else
		{
			currentmodel = currententity->model;
			if (!currentmodel)
			{
				R_DrawNullModel();
				continue;
			}
			switch (currentmodel->type)
			{
			case mod_alias:
				R_DrawAliasModel(currententity);
				break;
			case mod_brush:
				R_DrawBrushModel(currententity);
				break;
			case mod_sprite:
				R_DrawSpriteModel(currententity);
				break;
			default:
				ri.Sys_Error(ERR_DROP, "Bad modeltype");
				break;
			}
		}
	}

	// draw transparent entities
	// we could sort these if it ever becomes a problem...
	//qglDepthMask(0);		// no z writes
	for (i = 0; i < r_newrefdef.num_entities; i++)
	{
		currententity = &r_newrefdef.entities[i];
		if (!(currententity->flags & RF_TRANSLUCENT))
			continue;	// solid

		if (currententity->flags & RF_BEAM)
		{
			R_DrawBeam(currententity);
		}
		else
		{
			currentmodel = currententity->model;

			if (!currentmodel)
			{
				R_DrawNullModel();
				continue;
			}
			switch (currentmodel->type)
			{
			case mod_alias:
				R_DrawAliasModel(currententity);
				break;
			case mod_brush:
				R_DrawBrushModel(currententity);
				break;
			case mod_sprite:
				R_DrawSpriteModel(currententity);
				break;
			default:
				ri.Sys_Error(ERR_DROP, "Bad modeltype");
				break;
			}
		}
	}
	//qglDepthMask(1);		// back to writing

}

/*
** GL_DrawParticles
**
*/
void GL_DrawParticles(int num_particles, const particle_t particles[], const unsigned colortable[768])
{
	const particle_t* p;
	int				i;
	vec3_t			up, right;
	float			scale;
	byte			color[4];

	//GL_Bind(r_particletexture->texnum);
	//qglDepthMask(GL_FALSE);		// no z buffering
	//qglEnable(GL_BLEND);
	//GL_TexEnv(GL_MODULATE);
	//qglBegin(GL_TRIANGLES);

	VectorScale(vup, 1.5, up);
	VectorScale(vright, 1.5, right);

	ParticlesDefinitions pd;
	//pd.cbp.position_transform = renderer->GetModelView() * renderer->GetPerspective();
	pd.flags = PARTICLES_DEFAULT;
	pd.texture_index = r_particletexture->texnum;
	ParticlesVertex pv;
	for (p = particles, i = 0; i < num_particles; i++, p++)
	{
		// hack a scale up to keep particles from disapearing
		scale = (p->origin[0] - r_origin[0]) * vpn[0] +
			(p->origin[1] - r_origin[1]) * vpn[1] +
			(p->origin[2] - r_origin[2]) * vpn[2];

		if (scale < 20)
			scale = 1;
		else
			scale = 1 + scale * 0.004;

		*(int*)color = colortable[p->color];
		color[3] = p->alpha * 255;

		pv.color = { (float)color[0], (float)color[1], (float)color[2], (float)color[3] };
		
		//pv.texture_coord = { 0.0625, 0.0625 };
		pv.position = {p->origin[0], p->origin[1], p->origin[2]};

		pd.vert.push_back(pv);		
		
		//pv.texture_coord = { 1.0625, 0.0625 };
		pv.position = { p->origin[0] + up[0] * scale,
				        p->origin[1] + up[1] * scale,
				        p->origin[2] + up[2] * scale
		};

		pd.vert.push_back(pv);		

		//pv.texture_coord = { 0.0625, 1.0625 };
		pv.position = { p->origin[0] + right[0] * scale,
				p->origin[1] + right[1] * scale,
			    p->origin[2] + right[2] * scale
		};

		pd.vert.push_back(pv);

		//qglColor4ubv(color);
		//qglTexCoord2f(0.0625, 0.0625);
		//qglVertex3fv(p->origin);

		//qglTexCoord2f(1.0625, 0.0625);
		//qglVertex3f(p->origin[0] + up[0] * scale,
		//	p->origin[1] + up[1] * scale,
		//	p->origin[2] + up[2] * scale);

		//qglTexCoord2f(0.0625, 1.0625);
		//qglVertex3f(p->origin[0] + right[0] * scale,
		//	p->origin[1] + right[1] * scale,
		//	p->origin[2] + right[2] * scale);
	}

	if (num_particles > 0) {
		SmartTriangulation(&pd.ind, num_particles * 3, 0);
		particles_renderer->Add(pd);
	}
	//qglEnd();
	//qglDisable(GL_BLEND);
	//qglColor4f(1, 1, 1, 1);
	//qglDepthMask(1);		// back to normal Z buffering
	//GL_TexEnv(GL_REPLACE);
}

/*
===============
R_DrawParticles
===============
*/
void R_DrawParticles(void)
{
	//if (gl_ext_pointparameters->value && qglPointParameterfEXT)
	//{
		int i;
		unsigned char color[4];
		const particle_t* p;

		//qglDepthMask(GL_FALSE);
		//qglEnable(GL_BLEND);
		//qglDisable(GL_TEXTURE_2D);

		//qglPointSize(gl_particle_size->value);

		//qglBegin(GL_POINTS);
		ParticlesVertex part_vert;
		ParticlesDefinitions part_defs;
		part_defs.flags = PARTICLES_DEFAULT;
		for (i = 0, p = r_newrefdef.particles; i < r_newrefdef.num_particles; i++, p++)
		{
			*(int*)color = d_8to24table[p->color];
			color[3] = p->alpha * 255;

			part_vert.color = { (float)color[0], (float)color[1], (float)color[2],  (float)color[3] };
			part_vert.position = { p->origin[0], p->origin[1], p->origin[2] };
			part_defs.vert.push_back(part_vert);
			//qglColor4ubv(color);

			//qglVertex3fv(p->origin);
		}

		particles_renderer->Add(part_defs);
		//qglEnd();

		//qglDisable(GL_BLEND);
		//qglColor4f(1.0F, 1.0F, 1.0F, 1.0F);
		//qglDepthMask(GL_TRUE);
		//qglEnable(GL_TEXTURE_2D);

	//}
	//else
	//{
		//GL_DrawParticles(r_newrefdef.num_particles, r_newrefdef.particles, d_8to24table);
	//}
}

/*
============
R_PolyBlend
============
*/
void R_PolyBlend(void)
{
	using namespace DirectX;

	if (!v_blend[3]) {
		effects_renderer->is_render = false;
	} else {
		effects_renderer->is_render = true;
	}

	MODEL cb;
	if (effects_renderer->is_render) {
		cb.color.x = v_blend[0];
		cb.color.y = v_blend[1];
		cb.color.z = v_blend[2];
		cb.color.w = v_blend[3];
	}

	// z - 100
	cb.mod = XMMatrixScaling(vid.width, vid.height, 100);

	effects_renderer->Add(cb);
}

//=======================================================================

int SignbitsForPlane(cplane_t* out)
{
	int	bits, j;

	// for fast box on planeside test

	bits = 0;
	for (j = 0; j < 3; j++)
	{
		if (out->normal[j] < 0)
			bits |= 1 << j;
	}
	return bits;
}


void R_SetFrustum(void)
{
	int		i;

	// rotate VPN right by FOV_X/2 degrees
	RotatePointAroundVector(frustum[0].normal, vup, vpn, -(90 - r_newrefdef.fov_x / 2));
	// rotate VPN left by FOV_X/2 degrees
	RotatePointAroundVector(frustum[1].normal, vup, vpn, 90 - r_newrefdef.fov_x / 2);
	// rotate VPN up by FOV_X/2 degrees
	RotatePointAroundVector(frustum[2].normal, vright, vpn, 90 - r_newrefdef.fov_y / 2);
	// rotate VPN down by FOV_X/2 degrees
	RotatePointAroundVector(frustum[3].normal, vright, vpn, -(90 - r_newrefdef.fov_y / 2));

	for (i = 0; i < 4; i++)
	{
		frustum[i].type = PLANE_ANYZ;
		frustum[i].dist = DotProduct(r_origin, frustum[i].normal);
		frustum[i].signbits = SignbitsForPlane(&frustum[i]);
	}
}

// ========================================================================================================================

/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame(void)
{
	int i;
	mleaf_t* leaf;

	r_framecount++;

	// build the transformation matrix for the given view angles
	VectorCopy(r_newrefdef.vieworg, r_origin);

	AngleVectors(r_newrefdef.viewangles, vpn, vright, vup);

	// current viewcluster
	if (!(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
	{
		r_oldviewcluster = r_viewcluster;
		r_oldviewcluster2 = r_viewcluster2;
		leaf = Mod_PointInLeaf(r_origin, r_worldmodel);
		r_viewcluster = r_viewcluster2 = leaf->cluster;

		// check above and below so crossing solid water doesn't draw wrong
		if (!leaf->contents)
		{	// look down a bit
			vec3_t	temp;

			VectorCopy(r_origin, temp);
			temp[2] -= 16;
			leaf = Mod_PointInLeaf(temp, r_worldmodel);
			if (!(leaf->contents & CONTENTS_SOLID) &&
				(leaf->cluster != r_viewcluster2))
				r_viewcluster2 = leaf->cluster;
		}
		else
		{	// look up a bit
			vec3_t	temp;

			VectorCopy(r_origin, temp);
			temp[2] += 16;
			leaf = Mod_PointInLeaf(temp, r_worldmodel);
			if (!(leaf->contents & CONTENTS_SOLID) &&
				(leaf->cluster != r_viewcluster2))
				r_viewcluster2 = leaf->cluster;
		}
	}

	for (i = 0; i < 4; i++)
		v_blend[i] = r_newrefdef.blend[i];

	c_brush_polys = 0;
	c_alias_polys = 0;

	// clear out the portion of the screen that the NOWORLDMODEL defines
	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
	{
		//qglEnable(GL_SCISSOR_TEST);
		//qglClearColor(0.3, 0.3, 0.3, 1);
		//qglScissor(r_newrefdef.x, vid.height - r_newrefdef.height - r_newrefdef.y, r_newrefdef.width, r_newrefdef.height);
		//qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//qglClearColor(1, 0, 0.5, 0.5);
		//qglDisable(GL_SCISSOR_TEST);
	}
}

/*
=============
R_SetupGL
=============
*/
void R_SetupDX(void)
{
	using namespace DirectX;

	float	screenaspect;
	int		x, x2, y2, y, w, h;

	//
	// set up viewport
	//
	x = floor(r_newrefdef.x * vid.width / vid.width);
	x2 = ceil((r_newrefdef.x + r_newrefdef.width) * vid.width / vid.width);
	y = floor(vid.height - r_newrefdef.y * vid.height / vid.height);
	y2 = ceil(vid.height - (r_newrefdef.y + r_newrefdef.height) * vid.height / vid.height);

	w = x2 - x;
	h = y - y2;

	////
	//// set up projection matrix
	////

	screenaspect = (float)vid.width / (float)vid.height;

	cam.orthogonal = DirectX::XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0.0f, vid.width, vid.height, 0.0f, 0.0f, 1000.0f));
	cam.perspective = XMMatrixPerspectiveFovRH(XMConvertToRadians(r_newrefdef.fov_y), screenaspect, 4.0f, 4096.0f);
	//renderer->SetPerspectiveMatrix(r_newrefdef.fov_y, screenaspect, 4.0f, 4096.0f);

	//cam.weaponProj = XMMatrixIdentity() * XMMatrixScaling(-1.0f, 1.0f, 1.0f) * XMMatrixPerspectiveFovRH(XMConvertToRadians(r_newrefdef.fov_y), screenaspect, 4, 4096);

	// id's system:
	//	- X axis = Left/Right
	//	- Y axis = Forward/Backward
	//	- Z axis = Up/Down
	//
	// DirectX coordinate system:
	//	- X axis = Left/Right
	//	- Y axis = Up/Down
	//	- Z axis = Forward/Backward

	// vieworg - postition
	// viewangles - point view
	if (is_first) {
		cam.prev_view = XMMatrixIdentity();
		is_first = false;
	} else {
		cam.prev_view = XMMatrixMultiply(cam.perspective, cam.view);
	}

	cam.view = XMMatrixIdentity();
	cam.view *= XMMatrixTranslation(-r_newrefdef.vieworg[0], -r_newrefdef.vieworg[1], -r_newrefdef.vieworg[2]);

#if defined(DEBUG) || defined(_DEBUG)  
	printf("Camera coords: x = %f, y = %f, z = %f\n", r_newrefdef.vieworg[0], r_newrefdef.vieworg[1], r_newrefdef.vieworg[2]);
	single_point_light_x->value = r_newrefdef.vieworg[0];
	single_point_light_y->value = r_newrefdef.vieworg[1];
	single_point_light_z->value = r_newrefdef.vieworg[2];
#endif


	cam.view *= XMMatrixRotationZ(XMConvertToRadians(-r_newrefdef.viewangles[1]));
	cam.view *= XMMatrixRotationY(XMConvertToRadians(-r_newrefdef.viewangles[0]));
	cam.view *= XMMatrixRotationX(XMConvertToRadians(-r_newrefdef.viewangles[2]));

	cam.view *= XMMatrixRotationZ(XMConvertToRadians(90.0f));
	cam.view *= XMMatrixRotationX(XMConvertToRadians(-90.0f));

	//renderer->SetModelViewMatrix(cam.view);
	cam.view_projection_inverse = XMMatrixInverse(nullptr, XMMatrixMultiply(cam.perspective, cam.view));

	cam.fps = fps;
	cam.total_frames = frame_count;
	cam.resolution = XMFLOAT2(vid.width, vid.height);
	// Обновляем буфер
	cbCamera.Update(cam);
	cbCamera.Bind<CAMERA>(camera.slot);

	if (is_directional_light->value) {
		directional_light.color = { light_color_r->value, light_color_g->value, light_color_b->value, light_color_a->value };
		directional_light.direction = { light_direction_x->value, light_direction_y->value, light_direction_z->value };
		directional_light.intensity = light_intensity->value;
	}
	else {
		directional_light.color = { 0, 0, 0, 0 };
		directional_light.direction = { 0, 0, 0 };
		directional_light.intensity = 1.0f;
	}
	//directional_light.is_light = is_directional_light->value;
	cbDirectionalLight.Update(directional_light);
	cbDirectionalLight.Bind<DirectionalLight>(light.slot);

	
	//cbBuffer.Update(mbuffer);
	//cbBuffer.Bind<MatrixBuffer>(buffer.slot);

	bsp_renderer->InitCB();
	sky_renderer->InitCB();
	beam_renderer->InitCB();
	mod_renderer->InitCB();
	particles_renderer->InitCB();
	effects_renderer->InitCB();
	utils_renderer->InitCB();

	// Записываем моделвью матрицу в ворлд матрицу

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			r_world_matrix[(i * 4) + j] = cam.view.r[i].m128_f32[j];
		}
	}
}

/*
=============
R_Clear
=============
*/
void R_Clear(void)
{
	/*if (gl_ztrick->value)
	{
		static int trickframe;

		if (gl_clear->value)
			qglClear(GL_COLOR_BUFFER_BIT);

		trickframe++;
		if (trickframe & 1)
		{
			gldepthmin = 0;
			gldepthmax = 0.49999;
			qglDepthFunc(GL_LEQUAL);
		}
		else
		{
			gldepthmin = 1;
			gldepthmax = 0.5;
			qglDepthFunc(GL_GEQUAL);
		}
	}
	else
	{
		if (gl_clear->value)
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			qglClear(GL_DEPTH_BUFFER_BIT);
		gldepthmin = 0;
		gldepthmax = 1;
		qglDepthFunc(GL_LEQUAL);
	}

	qglDepthRange(gldepthmin, gldepthmax);*/

}

void R_Flash(void)
{
	R_PolyBlend();
}

/*
================
R_RenderView

r_newrefdef must be set before the first call
================
*/
void R_RenderView(refdef_t* fd)
{
	is_first = false;
	if (r_norefresh->value)
		return;

	r_newrefdef = *fd;

	if (!r_worldmodel && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
		ri.Sys_Error(ERR_DROP, "R_RenderView: NULL worldmodel");

	if (r_speeds->value)
	{
		c_brush_polys = 0;
		c_alias_polys = 0;
	}

	R_PushDlights();

	// Здесь по логике происходит ожидание пока исполнится все функции отрисовки на видюхе
	// подробности гуглятся по запросу - "glFinish"
	/*if (gl_finish->value)
		qglFinish();*/

	R_SetupFrame();

	R_SetFrustum();

	R_SetupDX();

	R_MarkLeaves();	// done here so we know if we're in water

	BEGIN_EVENT(L"BSP renderer");
	R_DrawWorld();

	R_DrawEntitiesOnList();

	R_RenderDlights();

	R_DrawParticles();

	R_DrawAlphaSurfaces();

	R_Flash();

	if (r_speeds->value)
	{
		ri.Con_Printf(PRINT_ALL, "%4i wpoly %4i epoly %i tex %i lmaps\n",
			c_brush_polys,
			c_alias_polys,
			c_visible_textures,
			c_visible_lightmaps);
	}
}


void	R_SetGL2D(void)
{
	// set 2D virtual screen size
	/*qglViewport(0, 0, vid.width, vid.height);
	qglMatrixMode(GL_PROJECTION);
	qglLoadIdentity();
	qglOrtho(0, vid.width, vid.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
	qglLoadIdentity();
	qglDisable(GL_DEPTH_TEST);
	qglDisable(GL_CULL_FACE);
	qglDisable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);
	qglColor4f(1, 1, 1, 1);*/
}

static void GL_DrawColoredStereoLinePair(float r, float g, float b, float y)
{
	/*qglColor3f(r, g, b);
	qglVertex2f(0, y);
	qglVertex2f(vid.width, y);
	qglColor3f(0, 0, 0);
	qglVertex2f(0, y + 1);
	qglVertex2f(vid.width, y + 1);*/
}

static void GL_DrawStereoPattern(void)
{
	/*int i;

	if (!(dx11_config.renderer & GL_RENDERER_INTERGRAPH))
		return;

	if (!dx11_state.stereo_enabled)
		return;

	R_SetGL2D();

	qglDrawBuffer(GL_BACK_LEFT);

	for (i = 0; i < 20; i++)
	{
		qglBegin(GL_LINES);
		GL_DrawColoredStereoLinePair(1, 0, 0, 0);
		GL_DrawColoredStereoLinePair(1, 0, 0, 2);
		GL_DrawColoredStereoLinePair(1, 0, 0, 4);
		GL_DrawColoredStereoLinePair(1, 0, 0, 6);
		GL_DrawColoredStereoLinePair(0, 1, 0, 8);
		GL_DrawColoredStereoLinePair(1, 1, 0, 10);
		GL_DrawColoredStereoLinePair(1, 1, 0, 12);
		GL_DrawColoredStereoLinePair(0, 1, 0, 14);
		qglEnd();

		GLimp_EndFrame();
	}*/
}


/*
====================
R_SetLightLevel

====================
*/
void R_SetLightLevel(void)
{
	vec3_t		shadelight;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	// save off light value for server to look at (BIG HACK!)

	R_LightPoint(r_newrefdef.vieworg, shadelight);

	// pick the greatest component, which should be the same
	// as the mono value returned by software
	if (shadelight[0] > shadelight[1])
	{
		if (shadelight[0] > shadelight[2])
			r_lightlevel->value = 150 * shadelight[0];
		else
			r_lightlevel->value = 150 * shadelight[2];
	}
	else
	{
		if (shadelight[1] > shadelight[2])
			r_lightlevel->value = 150 * shadelight[1];
		else
			r_lightlevel->value = 150 * shadelight[2];
	}

}

/*
@@@@@@@@@@@@@@@@@@@@@
R_RenderFrame

@@@@@@@@@@@@@@@@@@@@@
*/
void R_RenderFrame(refdef_t* fd)
{
	R_RenderView(fd);
	R_SetLightLevel();
	// settuing 2d render mode
	R_SetGL2D();
}

// reading param from config.txt
// setting a command
void R_Register(void)
{
	// their stuff
	gl_round_down = ri.Cvar_Get("gl_round_down", "1", 0);

	r_lefthand = ri.Cvar_Get("hand", "0", CVAR_USERINFO | CVAR_ARCHIVE);
	r_norefresh = ri.Cvar_Get("r_norefresh", "0", 0);
	r_fullbright = ri.Cvar_Get("r_fullbright", "0", 0);
	r_drawentities = ri.Cvar_Get("r_drawentities", "1", 0);
	r_drawworld = ri.Cvar_Get("r_drawworld", "1", 0);
	r_novis = ri.Cvar_Get("r_novis", "0", 0);
	r_nocull = ri.Cvar_Get("r_nocull", "0", 0);
	r_lerpmodels = ri.Cvar_Get("r_lerpmodels", "1", 0);
	r_speeds = ri.Cvar_Get("r_speeds", "0", 0);

	r_lightlevel = ri.Cvar_Get("r_lightlevel", "0", 0);
	gl_picmip = ri.Cvar_Get("gl_picmip", "0", 0);
	gl_skymip = ri.Cvar_Get("gl_skymip", "0", 0);
	// our stuff
	dx11_mode = ri.Cvar_Get("dx11_mode", "3", CVAR_ARCHIVE);
	taa = ri.Cvar_Get("taa", "1", 0);
	is_directional_light = ri.Cvar_Get("dirlight", "1", 0);
	light_color_r = ri.Cvar_Get("lightcolorr", "0.96862745098", 0);
	light_color_g = ri.Cvar_Get("lightcolorg", "0.36862745098", 0);
	light_color_b = ri.Cvar_Get("lightcolorb", "0.14509803921", 0);
	light_color_a = ri.Cvar_Get("lightcolora", "0.0", 0);
	light_direction_x = ri.Cvar_Get("lightdirx", "0.0", 0);
	light_direction_y = ri.Cvar_Get("lightdiry", "0.0", 0);
	light_direction_z = ri.Cvar_Get("lightdirz", "-1", 0);
	light_intensity = ri.Cvar_Get("lightintensity", "0.5", 0);
	light_sources = ri.Cvar_Get("lightsources", "0", 0);
	cs_lm = ri.Cvar_Get("cslm", "0", 0);
	positions = ri.Cvar_Get("positions", "0", 0);
	normals   = ri.Cvar_Get("normals", "0", 0);
	albedo    = ri.Cvar_Get("albedo", "0", 0);
	lightmap_index = ri.Cvar_Get("lmindex", "0", 0);
	single_point_light_x = ri.Cvar_Get("plightx", "0", 0);
	single_point_light_y = ri.Cvar_Get("plighty", "0", 0);
	single_point_light_z = ri.Cvar_Get("plightz", "0", 0);

	vid_fullscreen = ri.Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = ri.Cvar_Get("vid_gamma", "1.0", CVAR_ARCHIVE);
	vid_ref = ri.Cvar_Get("vid_ref", "gl", CVAR_ARCHIVE);

	ri.Cmd_AddCommand("cl", CompileShaders);
	ri.Cmd_AddCommand("imagelist", GL_ImageList_f);
	ri.Cmd_AddCommand("screenshot", GL_ScreenShot_f);
	ri.Cmd_AddCommand("modellist", Mod_Modellist_f);
}

bool R_SetMode(int* width, int* height)
{
	// can call before setting mode (width, height)
	// getting width and height regarding the installed mod
	if (!ri.Vid_GetModeInfo(width, height, dx11_mode->value))
	{
		ri.Con_Printf(PRINT_ALL, " invalid mode\n");
		printf("invalid mode\n");
		return false;
	}

	bool fullscreen;

	if (vid_fullscreen->modified)
	{
		ri.Cvar_SetValue("vid_fullscreen", vid_fullscreen->value);
		vid_fullscreen->modified = False;
	}

	fullscreen = vid_fullscreen->value;

	vid_fullscreen->modified = False;
	dx11_mode->modified = False;

	// set width and height in vid
	vid.height = *height;
	vid.width = *width;

	int posX = 0;
	int posY = 0;

	if (!fullscreen)
	{
		posX = (ri.Cvar_Get("vid_xpos", "0", 0))->value;
		posY = (ri.Cvar_Get("vid_ypos", "0", 0))->value;
	}
	
	// setting size of our window
	renderer->SetWindowMode(posX, posY, vid.width, vid.height, fullscreen);
	
	return true;
}

/*
===============
R_Init
===============
*/
qboolean R_Init(void* hinstance, void* hWnd)
{
	con::InitConsole();

	int width = 1280, height = 720;

	extern float r_turbsin[256];

	for (int j = 0; j < 256; j++)
	{
		r_turbsin[j] *= 0.5;
	}

	// setting palette mapped from 8 byte color to 24 byte color
	Draw_GetPalette();

	// register the cvar variables
	R_Register();

	// setting width, height and full-screen modes
	if (!R_SetMode(&width, &height))
	{
		return False;
	}

	// initialize DX11 context
	if (!renderer->Initialize((HINSTANCE)hinstance, (WNDPROC)hWnd)) {
		return False;
	}
	ui_renderer->Init();
	bsp_renderer->Init();
	sky_renderer->Init();
	beam_renderer->Init();
	mod_renderer->Init();
	particles_renderer->Init();
	effects_renderer->Init();
	utils_renderer->Init();

	Quad::vertex_buffer.Create(
		std::vector<UIVertex>{
			{ {0.0f, 0.0f}, {0.0f, 0.0f} },
			{ {1.0f, 0.0f}, {1.0f, 0.0f} },
			{ {1.0f, 1.0f}, {1.0f, 1.0f} },
			{ {0.0f, 1.0f}, {0.0f, 1.0f} }
		}
	);

	//renderer->AddCustomTextureToSrv("UV_cheker.png", 1590, true);
	
	Quad::index_buffer.Create({ 2, 1, 0, 0, 3, 2 });

	// Z-axis is null
	EffectsQuad::vertex_buffer.Create(
		std::vector<EffectsVertex>{
			{ {0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
			{ {1.0f, 0.0f, 0.0f }, {1.0f, 0.0f} },
			{ {1.0f, 1.0f, 0.0f }, {1.0f, 1.0f} },
			{ {0.0f, 1.0f, 0.0f }, {0.0f, 1.0f} }
	}
	);

	EffectsQuad::index_buffer.Create({ 2, 1, 0, 0, 3, 2 });

	SkyCube::vertex_buffer.Create(
		std::vector<SkyVertex>{
			{ {-1, -1, -1} },
			{ {-1,  1, -1} },
			{ { 1,  1, -1} },
			{ { 1, -1, -1} },

			{ {-1, -1,  1} },
			{ {-1,  1,  1} },
			{ { 1,  1,  1} },
			{ { 1, -1,  1} }
		}
	);

	SkyCube::index_buffer.Create(
		{
		 6, 7, 2, 7, 3, 2, 
		 1, 2, 0, 2, 3, 0, 
		 0, 5, 1, 0, 4, 5, 
		 4, 6, 5, 4, 7, 6, 
		 1, 5, 6, 1, 6, 2, 
		 0, 7, 4, 0, 3, 7  
		}
	);

	// let the sound and input subsystems know about the new window
	// can be call after creating windows and its context
	ri.Vid_NewWindow(width, height);

	// can be call after creating context and Vid_NewWindow()
	ri.Vid_MenuInit();

	DX_InitImages();
	Mod_Init();
	R_InitParticleTexture();
	Draw_InitLocal();

	// Init ortho matrix
	cam.orthogonal = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0.0f, vid.width, vid.height, 0.0f, 0.0f, 1000.0f));
	//renderer->InitMatrix(vid.width, vid.height);
	// Не должен создавать если уже есть
	cbCamera.Create(cam);
	cbCamera.Bind<CAMERA>(camera.slot);

	cbDirectionalLight.Create(directional_light);
	cbDirectionalLight.Bind<DirectionalLight>(light.slot);

	mbuffer.orthogonal = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 128, 128, 0.0f, 0.0f, 1000.0f));
	cbBuffer.Create(mbuffer);
	cbBuffer.Bind<MatrixBuffer>(buffer.slot);

	ID3DBlob* blob = CompileShader(L"ref_dx11\\shaders\\LightmapCS.hlsl", NULL, "CSMain", "cs_5_0");
	if (blob != nullptr)
		cs->Create(blob);
	//cs->Bind();

	return True;
}

/*
===============
R_Shutdown
===============
*/
void R_Shutdown(void)
{
	ri.Cmd_RemoveCommand("cl");
	ri.Cmd_RemoveCommand("imagelist");
	ri.Cmd_RemoveCommand("screenshot");
	ri.Cmd_RemoveCommand("modellist");

	// free models
	Mod_FreeAll();

	// shutdown images
	DX_ShutdownImages();

	/*
	** shut down OS specific DX12 stuff like contexts, etc.
	*/
	renderer->~Renderer();
}

// ==================================================================================================================================

/*
=============
R_SetPalette
=============
*/
unsigned r_rawpalette[256];

void R_SetPalette(const unsigned char* palette)
{
	int		i;

	byte* rp = (byte*)r_rawpalette;

	if (palette)
	{
		for (i = 0; i < 256; i++)
		{
			rp[i * 4 + 0] = palette[i * 3 + 0];
			rp[i * 4 + 1] = palette[i * 3 + 1];
			rp[i * 4 + 2] = palette[i * 3 + 2];
			rp[i * 4 + 3] = 0xff;
		}
	}
	else
	{
		for (i = 0; i < 256; i++)
		{
			rp[i * 4 + 0] = d_8to24table[i] & 0xff;
			rp[i * 4 + 1] = (d_8to24table[i] >> 8) & 0xff;
			rp[i * 4 + 2] = (d_8to24table[i] >> 16) & 0xff;
			rp[i * 4 + 3] = 0xff;
		}
	}
	// setting texture palette
	DX11_SetTexturePalette(r_rawpalette);
}

/*
@@@@@@@@@@@@@@@@@@@@@
R_BeginFrame
@@@@@@@@@@@@@@@@@@@@@
*/

void R_BeginFrame(float camera_separation)
{
	// here we need safe camera_separation

	if (dx11_mode->modified || vid_fullscreen->modified)
	{	// FIXME: only restart if CDS is required
		cvar_t* ref;

		ref = ri.Cvar_Get("vid_ref", "dx11", 0);
		ref->modified = True;
	}

	// 
	// GLimp_BeginFrame( camera_separation );

	// clear
	renderer->Clear();	

	bsp_renderer->lightmap_index = lightmap_index->value;
	bsp_renderer->point_light_buf = DirectX::XMFLOAT4(single_point_light_x->value, 
		single_point_light_y->value, single_point_light_z->value, 0);
}

/*
** GLimp_EndFrame
**
** Responsible for doing a swapbuffers and possibly for other stuff
** as yet to be determined.  Probably better not to make this a GLimp
** function and instead do a call to GLimp_SwapBuffers.
*/
void DX11_EndFrame(void)
{
	if (!is_first) {
		END_EVENT();

		BEGIN_EVENT(L"Model renderer");
		mod_renderer->Render();
		END_EVENT();

		BEGIN_EVENT(L"Beam renderer");
		beam_renderer->Render();
		END_EVENT();

		BEGIN_EVENT(L"Sky renderer");
		sky_renderer->Render();
		END_EVENT();

		BEGIN_EVENT(L"Particles renderer");
		particles_renderer->Render();
		END_EVENT();

		BEGIN_EVENT(L"BSP renderer (No dynamic lightmap)");
		bsp_renderer->Render();
		END_EVENT();		
		

		BEGIN_EVENT(L"Effects renderer");
		effects_renderer->fxaa = taa->value;
		effects_renderer->ChangeFlags(positions->value, normals->value, albedo->value);
		effects_renderer->Render();
		END_EVENT();

		if (light_sources->value && !bsp_renderer->is_first) {
			BEGIN_EVENT(L"Light source");
			utils_renderer->Render();
			END_EVENT();
		}

	}
	BEGIN_EVENT(L"UI renderer");
	ui_renderer->Render();
	END_EVENT();

	renderer->Swap();

	auto current_time = std::chrono::steady_clock::now();
	float delta_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - prev_time).count() / 1000000.0f;
	prev_time = current_time;

	total_time += delta_time;
	frame_count++;

	if (total_time > 1.0f) {
		fps = frame_count / total_time;

		total_time -= 1.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("Quake 2 DX11 | FPS: %f"), fps);
		SetWindowText(renderer->GetWindow(), text);

		frame_count = 0;
	}
}

/*
** GLimp_AppActivate
*/
void GLimp_AppActivate(qboolean active)
{
	return;
}

/*
** R_DrawBeam
*/
void R_DrawBeam(entity_t* e)
{
#define NUM_BEAM_SEGS 6

	int	i;
	float r, g, b;

	vec3_t perpvec;
	vec3_t direction, normalized_direction;
	vec3_t	start_points[NUM_BEAM_SEGS], end_points[NUM_BEAM_SEGS];
	vec3_t oldorigin, origin;

	oldorigin[0] = e->oldorigin[0];
	oldorigin[1] = e->oldorigin[1];
	oldorigin[2] = e->oldorigin[2];

	origin[0] = e->origin[0];
	origin[1] = e->origin[1];
	origin[2] = e->origin[2];

	normalized_direction[0] = direction[0] = oldorigin[0] - origin[0];
	normalized_direction[1] = direction[1] = oldorigin[1] - origin[1];
	normalized_direction[2] = direction[2] = oldorigin[2] - origin[2];

	if (VectorNormalize(normalized_direction) == 0)
		return;

	PerpendicularVector(perpvec, normalized_direction);
	VectorScale(perpvec, e->frame / 2, perpvec);

	for (i = 0; i < 6; i++)
	{
		RotatePointAroundVector(start_points[i], normalized_direction, perpvec, (360.0 / NUM_BEAM_SEGS) * i);
		VectorAdd(start_points[i], origin, start_points[i]);
		VectorAdd(start_points[i], direction, end_points[i]);
	}

	using namespace DirectX;
	std::vector<UINT> indexes;
	std::vector<BeamVertex> verts;
	BeamVertex vert;
	for (int i = 0; i < NUM_BEAM_SEGS; ++i) {
		vert.position = XMFLOAT3(start_points[i]);
		verts.push_back(vert);
		vert.position = XMFLOAT3(end_points[i]);
		verts.push_back(vert);
		vert.position = XMFLOAT3(start_points[(i + 1) % NUM_BEAM_SEGS]);
		verts.push_back(vert);
		vert.position = XMFLOAT3(end_points[(i + 1) % NUM_BEAM_SEGS]);
		verts.push_back(vert);
	}

	r = (d_8to24table[e->skinnum & 0xFF]) & 0xFF;
	g = (d_8to24table[e->skinnum & 0xFF] >> 8) & 0xFF;
	b = (d_8to24table[e->skinnum & 0xFF] >> 16) & 0xFF;

	r *= 1 / 255.0F;
	g *= 1 / 255.0F;
	b *= 1 / 255.0F;

	MODEL cb;
	cb.color.x = r;
	cb.color.y = g;
	cb.color.z = b;
	cb.color.w = e->alpha;

	BeamDefinitions beam_defs{
		verts, indexes, cb, BEAM_DEFAULT
	};

	beam_renderer->Add(beam_defs);
}

//===================================================================


void			R_BeginRegistration(char* map);
struct model_s* R_RegisterModel(char* name);
struct image_s* R_RegisterSkin(char* name);
void			R_SetSky(char* name, float rotate, vec3_t axis);
void			R_EndRegistration(void);

void			R_RenderFrame(refdef_t* fd);

struct image_s* Draw_FindPic(char* name);

void			Draw_Pic(int x, int y, char* name);
void			Draw_Char(int x, int y, int c);
void			Draw_TileClear(int x, int y, int w, int h, char* name);
void			Draw_Fill(int x, int y, int w, int h, int c);
void			Draw_FadeScreen(void);

/*
@@@@@@@@@@@@@@@@@@@@@
GetRefAPI

@@@@@@@@@@@@@@@@@@@@@
*/

refexport_t GetRefAPI(refimport_t rimp)
{
	refexport_t	re;

	ri = rimp;

	re.api_version = API_VERSION;

	re.BeginRegistration = R_BeginRegistration;
	re.RegisterModel = R_RegisterModel;
	re.RegisterSkin = R_RegisterSkin;
	re.RegisterPic = Draw_FindPic;
	re.SetSky = R_SetSky;
	re.EndRegistration = R_EndRegistration;

	re.RenderFrame = R_RenderFrame;

	re.DrawGetPicSize = Draw_GetPicSize;
	re.DrawPic = Draw_Pic;
	re.DrawStretchPic = Draw_StretchPic;
	re.DrawChar = Draw_Char;
	re.DrawTileClear = Draw_TileClear;
	re.DrawFill = Draw_Fill; 
	re.DrawFadeScreen = Draw_FadeScreen;

	re.DrawStretchRaw = Draw_StretchRaw;

	re.Init = R_Init;
	re.Shutdown = R_Shutdown;

	re.CinematicSetPalette = R_SetPalette;
	re.BeginFrame = R_BeginFrame;
	re.EndFrame = DX11_EndFrame;

	re.AppActivate = GLimp_AppActivate;

	Swap_Init();

	return re;
}

#ifndef REF_HARD_LINKED
// this is only here so the functions in q_shared.c and q_shwin.c can link
void Sys_Error(char* error, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	ri.Sys_Error(ERR_FATAL, (char*)"%s", text);
}

void Com_Printf(char* fmt, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start(argptr, fmt);
	vsprintf(text, fmt, argptr);
	va_end(argptr);

	ri.Con_Printf(PRINT_ALL, (char*)"%s", text);
}

#endif