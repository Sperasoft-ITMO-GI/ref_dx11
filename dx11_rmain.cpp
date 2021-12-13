// dx11r_main.c

#include "dx11_local.h"

viddef_t	vid;

refimport_t	ri;

cvar_t* dx11_mode;

cvar_t* vid_fullscreen;
cvar_t* vid_gamma;
cvar_t* vid_ref;

dx11config_t dx11_config;
dx11state_t  dx11_state;

InitDX11 dx11App = {};

void RecompileShaders()
{
	// TODO: added function to recompile shaders
	if(dx11App.CompileAllShaders())
		printf("RECOMPILE SUCCESS\n");
}

// reading param from config.txt
// setting a command
void R_Register(void)
{
	dx11_mode = ri.Cvar_Get("dx11_mode", "3", CVAR_ARCHIVE);

	vid_fullscreen = ri.Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = ri.Cvar_Get("vid_gamma", "1.0", CVAR_ARCHIVE);
	vid_ref = ri.Cvar_Get("vid_ref", "gl", CVAR_ARCHIVE);

	ri.Cmd_AddCommand("compile_shaders", RecompileShaders);
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
	dx11App.SetMode(*width, *height, fullscreen);

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
	if (!dx11App.InitializeDX11((HINSTANCE)hinstance, (WNDPROC)hWnd)) 
	{
		return False;
	}

	// let the sound and input subsystems know about the new window
	// can be call after creating windows and its context
	ri.Vid_NewWindow(width, height);

	// can be call after creating context and Vid_NewWindow()
	ri.Vid_MenuInit();

	return True;
}

/*
===============
R_Shutdown
===============
*/
void R_Shutdown(void)
{
	ri.Cmd_RemoveCommand((char*)"modellist");
	ri.Cmd_RemoveCommand((char*)"screenshot");
	ri.Cmd_RemoveCommand((char*)"imagelist");
	ri.Cmd_RemoveCommand((char*)"gl_strings");


	// free models
	//Mod_FreeAll();

	// shutdown images
	//GL_ShutdownImages();

	/*
	** shut down OS specific DX12 stuff like contexts, etc.
	*/
	dx11App.~InitDX11();
}

// ==================================================================================================================================
/*
@@@@@@@@@@@@@@@@@@@@@
R_BeginRegistration

Specifies the model that will be used as the world
@@@@@@@@@@@@@@@@@@@@@
*/
void R_BeginRegistration(char* model)
{
	return;
}

/*
@@@@@@@@@@@@@@@@@@@@@
R_RegisterModel

@@@@@@@@@@@@@@@@@@@@@
*/
struct model_s* R_RegisterModel(char* name)
{
	return NULL;
}

/*
===============
R_RegisterSkin
===============
*/
struct image_s* R_RegisterSkin(char* name)
{
	return NULL;
}

/*
============
R_SetSky
============
*/
// 3dstudio environment map names
char* suf[6] = { (char*)"rt", (char*)"bk", (char*)"lf", (char*)"ft", (char*)"up", (char*)"dn" };
void R_SetSky(char* name, float rotate, vec3_t axis)
{
	return;
}

/*
@@@@@@@@@@@@@@@@@@@@@
R_EndRegistration

@@@@@@@@@@@@@@@@@@@@@
*/
void R_EndRegistration(void)
{
	return;
}

/*
@@@@@@@@@@@@@@@@@@@@@
R_RenderFrame

@@@@@@@@@@@@@@@@@@@@@
*/
void R_RenderFrame(refdef_t* fd)
{
	return;
}

/*
================
Draw_Char

Draws one 8*8 graphics character with 0 being transparent.
It can be clipped to the top of the screen to allow the console to be
smoothly scrolled off.
================
*/
void Draw_Char(int x, int y, int num)
{
	return;
}

/*
=============
Draw_TileClear

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/
void Draw_TileClear(int x, int y, int w, int h, char* pic)
{
	return;
}

/*
=============
Draw_StretchRaw
=============
*/
extern unsigned	r_rawpalette[256];

void Draw_StretchRaw(int x, int y, int w, int h, int cols, int rows, byte* data)
{
	return;
}

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

	// TODO: just clear color buffers
	// maybe this is don't need to us
	//qglClearColor(0, 0, 0, 0);
	//qglClear(GL_COLOR_BUFFER_BIT);
	//qglClearColor(1, 0, 0.5, 0.5);
}

/*
@@@@@@@@@@@@@@@@@@@@@
R_BeginFrame
@@@@@@@@@@@@@@@@@@@@@
*/
void R_BeginFrame(float camera_separation)
{
	dx11App.ClearScene();
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
	dx11App.SwapBuffers();
}

/*
** GLimp_AppActivate
*/
void GLimp_AppActivate(qboolean active)
{
	return;
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