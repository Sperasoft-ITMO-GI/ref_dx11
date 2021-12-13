#pragma once

#include <windows.h>
#include <assert.h>

#include <stdio.h>

#include <math.h>

// specific things for Q2_Engine
#include "../client/ref.h"
#include "../win32/winquake.h"

#include "dx11_init.h"


#define	REF_VERSION	"DX11 0.01"

extern unsigned	d_8to24table[256];

typedef struct
{
	unsigned		width, height;			// coordinates from main game
} viddef_t;

extern	viddef_t	vid;

extern cvar_t* dx11_mode;

extern cvar_t* vid_fullscreen;
extern cvar_t* vid_gamma;
extern cvar_t* vid_ref;

extern InitDX11 dx11App;

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


#define	TEXNUM_LIGHTMAPS	1024
#define	TEXNUM_SCRAPS		1152
#define	TEXNUM_IMAGES		1153

#define		MAX_GLTEXTURES	1024

#define	MAX_LBM_HEIGHT		480

#define BACKFACE_EPSILON	0.01


//====================================================

extern	image_t		gltextures[MAX_GLTEXTURES];
extern	int			numgltextures;

extern	int		registration_sequence;


extern	image_t* r_notexture;
extern	image_t* r_particletexture;
extern	entity_t* currententity;
//extern	model_t* currentmodel;
extern	int			r_visframecount;
extern	int			r_framecount;
extern	cplane_t	frustum[4];
extern	int			c_brush_polys, c_alias_polys;


extern	int			gl_filter_min, gl_filter_max;



void LoadPCX(char* filename, byte** pic, byte** palette, int* width, int* height);


bool R_SetMode(int* width, int* height);

int Draw_GetPalette(void);

void DX11_SetTexturePalette(unsigned palette[256]);


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
void	R_SetPalette(const unsigned char* palette);

struct image_s* R_RegisterSkin(char* name);

/*
====================================================================

IMPORTED FUNCTIONS

====================================================================
*/

extern	refimport_t	ri;