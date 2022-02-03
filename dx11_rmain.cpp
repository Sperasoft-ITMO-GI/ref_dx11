// dx11r_main.c

#define _XM_NO_INTRINSICS_

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

cvar_t* vid_fullscreen;
cvar_t* vid_gamma;
cvar_t* vid_ref;

dx11config_t dx11_config;
dx11state_t  dx11_state;

Renderer* renderer = Renderer::GetInstance();
UIRenderer* ui_renderer =   new UIRenderer(); // Зачем указатель? Ответа на этот вопрос у меня нет...
BSPRenderer* bsp_renderer = new BSPRenderer();
SkyRenderer* sky_renderer = new SkyRenderer();

States* States::states = nullptr;

cvar_t* gl_round_down;


void RecompileShaders()
{
	// TODO: added function to recompile shaders
	//if(dx11App.CompileAllShaders())
		//printf("RECOMPILE SUCCESS\n");
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


void R_RotateForEntity(entity_t* e)
{
	/*qglTranslatef(e->origin[0], e->origin[1], e->origin[2]);

	qglRotatef(e->angles[1], 0, 0, 1);
	qglRotatef(-e->angles[0], 0, 1, 0);
	qglRotatef(-e->angles[2], 1, 0, 0);*/
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

	/*if (alpha != 1.0F)
		qglEnable(GL_BLEND);

	qglColor4f(1, 1, 1, alpha);

	GL_Bind(currentmodel->skins[e->frame]->texnum);

	GL_TexEnv(GL_MODULATE);

	if (alpha == 1.0)
		qglEnable(GL_ALPHA_TEST);
	else
		qglDisable(GL_ALPHA_TEST);

	qglBegin(GL_QUADS);

	qglTexCoord2f(0, 1);
	VectorMA(e->origin, -frame->origin_y, up, point);
	VectorMA(point, -frame->origin_x, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(0, 0);
	VectorMA(e->origin, frame->height - frame->origin_y, up, point);
	VectorMA(point, -frame->origin_x, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(1, 0);
	VectorMA(e->origin, frame->height - frame->origin_y, up, point);
	VectorMA(point, frame->width - frame->origin_x, right, point);
	qglVertex3fv(point);

	qglTexCoord2f(1, 1);
	VectorMA(e->origin, -frame->origin_y, up, point);
	VectorMA(point, frame->width - frame->origin_x, right, point);
	qglVertex3fv(point);

	qglEnd();

	qglDisable(GL_ALPHA_TEST);
	GL_TexEnv(GL_REPLACE);

	if (alpha != 1.0F)
		qglDisable(GL_BLEND);

	qglColor4f(1, 1, 1, 1);*/
	VectorMA(e->origin, -frame->origin_y, up, point);
	VectorMA(point, frame->width - frame->origin_x, right, point);
	using namespace DirectX;
	ConstantBufferQuad cbq;
	//cbq.position_transform *= XMMatrixTranspose(XMMatrixScaling(8, 8, 0) * XMMatrixTranslation(x, y, 0) * renderer->GetOrthogonal());
	cbq.texture_transform *= XMMatrixTranslation(0, 1, 0);

	ConstantBuffer<ConstantBufferQuad> cb(cbq);
	//Quad textured_quad(cb, UI_TEXTURED, draw_chars->texnum);
	//ui_renderer->AddElement(textured_quad);
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
	//const particle_t* p;
	//int				i;
	//vec3_t			up, right;
	//float			scale;
	//byte			color[4];

	//GL_Bind(r_particletexture->texnum);
	//qglDepthMask(GL_FALSE);		// no z buffering
	//qglEnable(GL_BLEND);
	//GL_TexEnv(GL_MODULATE);
	//qglBegin(GL_TRIANGLES);

	//VectorScale(vup, 1.5, up);
	//VectorScale(vright, 1.5, right);

	//for (p = particles, i = 0; i < num_particles; i++, p++)
	//{
	//	// hack a scale up to keep particles from disapearing
	//	scale = (p->origin[0] - r_origin[0]) * vpn[0] +
	//		(p->origin[1] - r_origin[1]) * vpn[1] +
	//		(p->origin[2] - r_origin[2]) * vpn[2];

	//	if (scale < 20)
	//		scale = 1;
	//	else
	//		scale = 1 + scale * 0.004;

	//	*(int*)color = colortable[p->color];
	//	color[3] = p->alpha * 255;

	//	qglColor4ubv(color);

	//	qglTexCoord2f(0.0625, 0.0625);
	//	qglVertex3fv(p->origin);

	//	qglTexCoord2f(1.0625, 0.0625);
	//	qglVertex3f(p->origin[0] + up[0] * scale,
	//		p->origin[1] + up[1] * scale,
	//		p->origin[2] + up[2] * scale);

	//	qglTexCoord2f(0.0625, 1.0625);
	//	qglVertex3f(p->origin[0] + right[0] * scale,
	//		p->origin[1] + right[1] * scale,
	//		p->origin[2] + right[2] * scale);
	//}

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
	/*if (gl_ext_pointparameters->value && qglPointParameterfEXT)
	{
		int i;
		unsigned char color[4];
		const particle_t* p;

		qglDepthMask(GL_FALSE);
		qglEnable(GL_BLEND);
		qglDisable(GL_TEXTURE_2D);

		qglPointSize(gl_particle_size->value);

		qglBegin(GL_POINTS);
		for (i = 0, p = r_newrefdef.particles; i < r_newrefdef.num_particles; i++, p++)
		{
			*(int*)color = d_8to24table[p->color];
			color[3] = p->alpha * 255;

			qglColor4ubv(color);

			qglVertex3fv(p->origin);
		}
		qglEnd();

		qglDisable(GL_BLEND);
		qglColor4f(1.0F, 1.0F, 1.0F, 1.0F);
		qglDepthMask(GL_TRUE);
		qglEnable(GL_TEXTURE_2D);

	}
	else
	{
		GL_DrawParticles(r_newrefdef.num_particles, r_newrefdef.particles, d_8to24table);
	}*/
}

/*
============
R_PolyBlend
============
*/
void R_PolyBlend(void)
{
	//if (!gl_polyblend->value)
	//	return;
	//if (!v_blend[3])
	//	return;

	//qglDisable(GL_ALPHA_TEST);
	//qglEnable(GL_BLEND);
	//qglDisable(GL_DEPTH_TEST);
	//qglDisable(GL_TEXTURE_2D);

	//qglLoadIdentity();

	//// FIXME: get rid of these
	//qglRotatef(-90, 1, 0, 0);	    // put Z going up
	//qglRotatef(90, 0, 0, 1);	    // put Z going up

	//qglColor4fv(v_blend);

	//qglBegin(GL_QUADS);

	//qglVertex3f(10, 100, 100);
	//qglVertex3f(10, -100, 100);
	//qglVertex3f(10, -100, -100);
	//qglVertex3f(10, 100, -100);
	//qglEnd();

	//qglDisable(GL_BLEND);
	//qglEnable(GL_TEXTURE_2D);
	//qglEnable(GL_ALPHA_TEST);

	//qglColor4f(1, 1, 1, 1);
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
	float	screenaspect;
	//float	yfov;
	int		x, x2, y2, y, w, h;

	// Тут каждый раз устанавливается новый вьюпорт
	// Не ясно, нужно ли это делать

	//
	// set up viewport
	//
	x = floor(r_newrefdef.x * vid.width / vid.width);
	x2 = ceil((r_newrefdef.x + r_newrefdef.width) * vid.width / vid.width);
	y = floor(vid.height - r_newrefdef.y * vid.height / vid.height);
	y2 = ceil(vid.height - (r_newrefdef.y + r_newrefdef.height) * vid.height / vid.height);

	w = x2 - x;
	h = y - y2;

	//qglViewport(x, y2, w, h);

	////
	//// set up projection matrix
	////
	
	screenaspect = (float)r_newrefdef.width / r_newrefdef.height;
	renderer->SetPerspectiveMatrix(r_newrefdef.fov_y, screenaspect, 4.0f, 4096.0f);

	//qglMatrixMode(GL_PROJECTION);
	//qglLoadIdentity();
	//MYgluPerspective(r_newrefdef.fov_y, screenaspect, 4, 4096);

	//qglCullFace(GL_FRONT);

	// Тут считаем матрицу моделвью

	//qglMatrixMode(GL_MODELVIEW);
	//qglLoadIdentity();

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

	using namespace DirectX;

	XMMATRIX model_view = XMMatrixIdentity();
	/*model_view *= XMMatrixRotationX(XMConvertToRadians(-90.0f));
	model_view *= XMMatrixRotationZ(XMConvertToRadians(90.0f));

	model_view *= XMMatrixRotationX(XMConvertToRadians(-r_newrefdef.viewangles[2]));
	model_view *= XMMatrixRotationY(XMConvertToRadians(-r_newrefdef.viewangles[0]));
	model_view *= XMMatrixRotationZ(XMConvertToRadians(-r_newrefdef.viewangles[1]));

	model_view *= XMMatrixTranslation(-r_newrefdef.vieworg[0], -r_newrefdef.vieworg[1], -r_newrefdef.vieworg[2]);*/


	model_view *= XMMatrixTranslation(-r_newrefdef.vieworg[0], -r_newrefdef.vieworg[1], -r_newrefdef.vieworg[2]);

	model_view *= XMMatrixRotationZ(XMConvertToRadians(-r_newrefdef.viewangles[1]));
	model_view *= XMMatrixRotationY(XMConvertToRadians(-r_newrefdef.viewangles[0]));
	model_view *= XMMatrixRotationX(XMConvertToRadians(-r_newrefdef.viewangles[2]));

	model_view *= XMMatrixRotationZ(XMConvertToRadians(90.0f));
	model_view *= XMMatrixRotationX(XMConvertToRadians(-90.0f));

	//model_view = XMMatrixTranspose(model_view);

	renderer->SetModelViewMatrix(model_view);

	//qglRotatef(-90, 1, 0, 0);	    // put Z going up
	//qglRotatef(90, 0, 0, 1);	    // put Z going up
	//qglRotatef(-r_newrefdef.viewangles[2], 1, 0, 0);
	//qglRotatef(-r_newrefdef.viewangles[0], 0, 1, 0);
	//qglRotatef(-r_newrefdef.viewangles[1], 0, 0, 1);
	//qglTranslatef(-r_newrefdef.vieworg[0], -r_newrefdef.vieworg[1], -r_newrefdef.vieworg[2]);

	// Записываем моделвью матрицу в ворлд матрицу
	// Может быть нужно траспанировать

	for (int i = 0; i < 16; ++i) {
		r_world_matrix[i] = model_view.m[i / 4][i % 4];
	}
	//qglGetFloatv(GL_MODELVIEW_MATRIX, r_world_matrix);

	////
	//// set drawing parms
	////
	//if (gl_cull->value)
	//	qglEnable(GL_CULL_FACE);
	//else
	//	qglDisable(GL_CULL_FACE);

	//qglDisable(GL_BLEND);
	//qglDisable(GL_ALPHA_TEST);
	//qglEnable(GL_DEPTH_TEST);
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

	vid_fullscreen = ri.Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = ri.Cvar_Get("vid_gamma", "1.0", CVAR_ARCHIVE);
	vid_ref = ri.Cvar_Get("vid_ref", "gl", CVAR_ARCHIVE);

	ri.Cmd_AddCommand("compile_shaders", RecompileShaders);
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

	// setting size of our window
	renderer->SetWindowMode(vid.width, vid.height, fullscreen);
	
	return true;
}

/*
===============
R_Init
===============
*/
qboolean R_Init(void* hinstance, void* hWnd)
{
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

	Quad::vb.Create(
		std::vector<UIVertex>{
			{ {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
			{ {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
			{ {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
			{ {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} }
		}
	);
	
	Quad::ib.Create({ 2, 1, 0, 0, 3, 2 });

	// let the sound and input subsystems know about the new window
	// can be call after creating windows and its context
	ri.Vid_NewWindow(width, height);

	// can be call after creating context and Vid_NewWindow()
	ri.Vid_MenuInit();

	DX_InitImages();
	Mod_Init();
	R_InitParticleTexture();
	Draw_InitLocal();

	return True;
}

/*
===============
R_Shutdown
===============
*/
void R_Shutdown(void)
{
	ri.Cmd_RemoveCommand("compile_shaders");
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
	bsp_renderer->Render();
	sky_renderer->Render();

	renderer->UnSetDepthBuffer();
	ui_renderer->Render();
	renderer->SetDepthBuffer();
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
	renderer->Swap();
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

	/*qglDisable(GL_TEXTURE_2D);
	qglEnable(GL_BLEND);
	qglDepthMask(GL_FALSE);

	r = (d_8to24table[e->skinnum & 0xFF]) & 0xFF;
	g = (d_8to24table[e->skinnum & 0xFF] >> 8) & 0xFF;
	b = (d_8to24table[e->skinnum & 0xFF] >> 16) & 0xFF;

	r *= 1 / 255.0F;
	g *= 1 / 255.0F;
	b *= 1 / 255.0F;

	qglColor4f(r, g, b, e->alpha);

	qglBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i < NUM_BEAM_SEGS; i++)
	{
		qglVertex3fv(start_points[i]);
		qglVertex3fv(end_points[i]);
		qglVertex3fv(start_points[(i + 1) % NUM_BEAM_SEGS]);
		qglVertex3fv(end_points[(i + 1) % NUM_BEAM_SEGS]);
	}
	qglEnd();

	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
	qglDepthMask(GL_TRUE);*/
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