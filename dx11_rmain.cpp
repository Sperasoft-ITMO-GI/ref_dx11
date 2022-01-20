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

//InitDX11 dx11App = {};
Renderer* renderer = Renderer::GetInstance();
UIRenderer* ui_renderer = new UIRenderer(); // Зачем указатель? Ответа на этот вопрос у меня нет...

States* States::states = nullptr;

cvar_t* gl_round_down;

void RecompileShaders()
{
	// TODO: added function to recompile shaders
	//if(dx11App.CompileAllShaders())
		//printf("RECOMPILE SUCCESS\n");
}

// reading param from config.txt
// setting a command
void R_Register(void)
{
	dx11_mode = ri.Cvar_Get("dx11_mode", "3", CVAR_ARCHIVE);

	vid_fullscreen = ri.Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = ri.Cvar_Get("vid_gamma", "1.0", CVAR_ARCHIVE);
	vid_ref = ri.Cvar_Get("vid_ref", "gl", CVAR_ARCHIVE);

	gl_round_down = ri.Cvar_Get("gl_round_down", "1", 0);

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
	//dx11App.SetMode(*width, *height, fullscreen);
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
	//if (!dx11App.InitializeDX11((HINSTANCE)hinstance, (WNDPROC)hWnd)) 
	//{
	//	return False;
	//}
	if (!renderer->Initialize((HINSTANCE)hinstance, (WNDPROC)hWnd)) {
		return False;
	}
	ui_renderer->Init();
	Quad::vb.Create(
		std::vector<UIVertex>{
			{ {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{ {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{ {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{ {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
		}
	);
	
	Quad::ib.Create({2, 1, 0, 0, 3, 2});
	


	// let the sound and input subsystems know about the new window
	// can be call after creating windows and its context
	ri.Vid_NewWindow(width, height);

	// can be call after creating context and Vid_NewWindow()
	ri.Vid_MenuInit();

	DX_InitImages();
	//Mod_Init();
	//R_InitParticleTexture();
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

	// free models
	//Mod_FreeAll();

	// shutdown images
	DX_ShutdownImages();

	/*
	** shut down OS specific DX12 stuff like contexts, etc.
	*/
	//dx11App.~InitDX11();
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
	if (dx11_mode->modified || vid_fullscreen->modified)
	{	// FIXME: only restart if CDS is required
		cvar_t* ref;

		ref = ri.Cvar_Get("vid_ref", "dx11", 0);
		ref->modified = True;
	}
	renderer->Clear();
	ui_renderer->Render();
	
	//dx11App.ClearScene();
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
	//dx11App.SwapBuffers();
	
	renderer->Swap();
}

/*
** GLimp_AppActivate
*/
void GLimp_AppActivate(qboolean active)
{
	return;
}

// В этой функции вызываются все функции для отрисовки игры

//void R_RenderView(refdef_t* fd)
//{
//	if (r_norefresh->value)
//		return;
//
//	r_newrefdef = *fd;
//
//	if (!r_worldmodel && !(r_newrefdef.rdflags & RDF_NOWORLDMODEL))
//		ri.Sys_Error(ERR_DROP, "R_RenderView: NULL worldmodel");
//
//	if (r_speeds->value)
//	{
//		c_brush_polys = 0;
//		c_alias_polys = 0;
//	}
//
//	R_PushDlights();
//
//	if (gl_finish->value)
//		qglFinish();
//
//	R_SetupFrame();
//
//	R_SetFrustum();
//
//	R_SetupGL();
//
//	R_MarkLeaves();	// done here so we know if we're in water
//
//	R_DrawWorld();
//
//	R_DrawEntitiesOnList();
//
//	R_RenderDlights();
//
//	R_DrawParticles();
//
//	R_DrawAlphaSurfaces();
//
//	R_Flash();
//
//	if (r_speeds->value)
//	{
//		ri.Con_Printf(PRINT_ALL, "%4i wpoly %4i epoly %i tex %i lmaps\n",
//			c_brush_polys,
//			c_alias_polys,
//			c_visible_textures,
//			c_visible_lightmaps);
//	}
//}

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