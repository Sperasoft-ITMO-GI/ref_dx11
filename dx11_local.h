#pragma once

#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <set>

// specific things for Q2_Engine
#include <../client/ref.h>
#include <../win32/winquake.h>

#include <RendererIncludes.h>




#define	REF_VERSION	"DX11 0.01"

extern unsigned	d_8to24table[256];

typedef struct
{
	unsigned		width, height;			// coordinates from main game
} viddef_t;

extern	viddef_t	vid;

extern cvar_t* dx11_mode;
extern cvar_t* taa;
extern cvar_t* is_directional_light;
extern cvar_t* light_color_r;
extern cvar_t* light_color_g;
extern cvar_t* light_color_b;
extern cvar_t* light_color_a;
extern cvar_t* light_direction_x;
extern cvar_t* light_direction_y;
extern cvar_t* light_direction_z;
extern cvar_t* light_intensity;
extern cvar_t* light_sources;
extern cvar_t* cs_lm;
extern cvar_t* positions;
extern cvar_t* normals;
extern cvar_t* albedo;
extern cvar_t* lightmap_index;
extern cvar_t* single_point_light_x;
extern cvar_t* single_point_light_y;
extern cvar_t* single_point_light_z;

extern CAMERA cam;

extern cvar_t* vid_fullscreen;
extern cvar_t* vid_gamma;
extern cvar_t* vid_ref;

extern cvar_t* gl_skymip;
extern cvar_t* gl_picmip;

extern	cvar_t* intensity;

extern	int		c_visible_lightmaps;
extern	int		c_visible_textures;

extern	float	r_world_matrix[16];

extern Renderer*          renderer;
extern UIRenderer*        ui_renderer;
extern BSPRenderer*       bsp_renderer;
extern SkyRenderer*       sky_renderer;
extern BeamRenderer*      beam_renderer;
extern ModRenderer*       mod_renderer;
extern ParticlesRenderer* particles_renderer;
extern EffectsRenderer*   effects_renderer;
extern UtilsRenderer*     utils_renderer;
extern ComputeShader* cs;

extern MatrixBuffer mbuffer;
extern ConstantBuffer<MatrixBuffer> cbBuffer;

extern bool multiTexture;

/*

  skins will be outline flood filled and mip mapped
  pics and sprites with alpha will be outline flood filled
  pic won't be mip mapped

  model skin
  sprite frame
  wall texture
  pic

*/

typedef enum
{
	it_skin,
	it_sprite,
	it_wall,
	it_pic,
	it_sky
} imagetype_t;

typedef struct image_s
{
	char	name[MAX_QPATH];			// game path, including extension
	imagetype_t	type;
	int		width, height;				// source image
	int		upload_width, upload_height;	// after power of two and picmip
	int		registration_sequence;		// 0 = free
	struct msurface_s* texturechain;	// for sort-by-texture world drawing
	int		texnum;						// gl texture binding
	float	sl, tl, sh, th;				// 0,0 - 1,1 unless part of the scrap
	qboolean	scrap;
	qboolean	has_alpha;

	qboolean paletted;
} image_t;

typedef struct
{
	int         renderer;
	const char* renderer_string;
	const char* vendor_string;
	const char* version_string;
	const char* extensions_string;

	qboolean	allow_cds;
} dx11config_t;

typedef struct
{
	float inverse_intensity;
	qboolean fullscreen;

	int     prev_mode;

	unsigned char* d_16to8table;

	int lightmap_textures;

	int	currenttextures[2];
	int currenttmu;

	float camera_separation;
	qboolean stereo_enabled;

	unsigned char originalRedGammaTable[256];
	unsigned char originalGreenGammaTable[256];
	unsigned char originalBlueGammaTable[256];
} dx11state_t;

extern dx11config_t  dx11_config;
extern dx11state_t   dx11_state;

typedef enum
{
	rserr_ok,

	rserr_invalid_fullscreen,
	rserr_invalid_mode,

	rserr_unknown
} rserr_t;

#include "dx11_model.h"

// our texture counter
#define MAX_DXTEXTURES 1600
#define MAX_DXSCRAPTEXTURES 400
#define CINEMATIC_PICTURE 1300

// 1024 this a number from OpenGL, this may call error
#define	TEXNUM_LIGHTMAPS	1024

extern	image_t		dxtextures[MAX_DXTEXTURES];
extern	int			numdxtextures;
extern	int			scrapNum;



#define	MAX_LBM_HEIGHT		480

#define BACKFACE_EPSILON	0.01


//====================================================

extern	int		registration_sequence;


extern	image_t* r_notexture;
extern	image_t* r_particletexture;
extern	entity_t* currententity;
extern	model_t* currentmodel;
extern	int			r_visframecount;
extern	int			r_framecount;
extern	cplane_t	frustum[4];
extern	int			c_brush_polys, c_alias_polys;


extern	int			gl_filter_min, gl_filter_max;

//
// view origin
//
extern	vec3_t	vup;
extern	vec3_t	vpn;
extern	vec3_t	vright;
extern	vec3_t	r_origin;

//
// screen size info
//
extern	refdef_t r_newrefdef;
extern	int		 r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

extern	cvar_t* r_norefresh;
extern	cvar_t* r_lefthand;
extern	cvar_t* r_drawentities;
extern	cvar_t* r_drawworld;
extern	cvar_t* r_speeds;
extern	cvar_t* r_fullbright;
extern	cvar_t* r_novis;
extern	cvar_t* r_nocull;
extern	cvar_t* r_lerpmodels;

extern	cvar_t* r_lightlevel;	// FIXME: This is a HACK to get the client's light level

extern	cvar_t* gl_round_down;


void Draw_InitLocal(void);

void R_RenderView(refdef_t* fd);

void LoadPCX(char* filename, byte** pic, byte** palette, int* width, int* height);

void DX_ShutdownImages(void);
void DX_InitImages(void);

void DX_FreeUnusedImages(void);

bool R_SetMode(int* width, int* height);

int Draw_GetPalette(void);

void DX11_SetTexturePalette(unsigned palette[256]);

void R_TranslatePlayerSkin(int playernum);
void GL_Bind(int texnum);
//void GL_MBind(GLenum target, int texnum);
//void GL_TexEnv(GLenum value);
//void GL_EnableMultitexture(qboolean enable);
//void GL_SelectTexture(GLenum);

void R_LightPoint(vec3_t p, vec3_t color);
void R_PushDlights(void);

extern	model_t* r_worldmodel;

extern	unsigned	d_8to24table[256];

extern	int		registration_sequence;

extern float colorBuf[4];


void V_AddBlend(float r, float g, float b, float a, float* v_blend);

void R_RenderView(refdef_t* fd);
void GL_ScreenShot_f(void);
void R_DrawAliasModel(entity_t* e);
void R_DrawBrushModel(entity_t* e);
void R_DrawSpriteModel(entity_t* e);
void R_DrawBeam(entity_t* e);
void R_DrawWorld(void);
void R_RenderDlights(void);
void R_DrawAlphaSurfaces(void);
void R_InitParticleTexture(void);
void Draw_InitLocal(void);
void GL_SubdivideSurface(msurface_t* fa);
qboolean R_CullBox(vec3_t mins, vec3_t maxs);
DirectX::XMMATRIX R_RotateForEntity(entity_t * e);
void R_MarkLeaves(void);

glpoly_t* WaterWarpPolyVerts(glpoly_t* p);
void EmitWaterPolys(msurface_t* fa);
void R_AddSkySurface(msurface_t* fa);
void R_ClearSkyBox(void);
void R_DrawSkyBox(void);
void R_MarkLights(dlight_t* light, int bit, mnode_t* node);

void COM_StripExtension(char* in, char* out);

void GL_ResampleTexture(unsigned* in, int inwidth, int inheight, unsigned* out, int outwidth, int outheight);

struct image_s* R_RegisterSkin(char* name);

void	GL_TextureMode(char* string);
void	GL_ImageList_f(void);

void GL_TextureAlphaMode(char* string);
void GL_TextureSolidMode(char* string);

void LoadPCX(char* filename, byte** pic, byte** palette, int* width, int* height);
image_t* DX_LoadPic(char* name, byte* pic, int width, int height, imagetype_t type, int bits);
image_t* DX11_FindImage(char* name, imagetype_t type);

qboolean 	R_Init(void* hinstance, void* hWnd);
void		R_Shutdown(void);

void	Draw_GetPicSize(int* w, int* h, char* name);
void	Draw_Pic(int x, int y, char* name);
void	Draw_StretchPic(int x, int y, int w, int h, char* name);
void	Draw_Char(int x, int y, int c);
void	Draw_TileClear(int x, int y, int w, int h, char* name);
void	Draw_Fill(int x, int y, int w, int h, int c);
void	Draw_FadeScreen(void);
void	Draw_StretchRaw(int x, int y, int w, int h, int cols, int rows, byte* data);

void	R_BeginFrame(float camera_separation);
void	R_SwapBuffers(int);
void	R_SetPalette(const unsigned char* palette);

struct image_s* R_RegisterSkin(char* name);

/*
====================================================================

IMPORTED FUNCTIONS

====================================================================
*/

extern	refimport_t	ri;