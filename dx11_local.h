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

//unsigned	d_8to24table[256];

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

typedef struct
{

} dx12config_t;

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
} dx12state_t;

extern dx12config_t  gl_config;
extern dx12state_t   gl_state;

typedef enum
{
	rserr_ok,

	rserr_invalid_fullscreen,
	rserr_invalid_mode,

	rserr_unknown
} rserr_t;

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