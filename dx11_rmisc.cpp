// r_misc.c

#include <dx11_local.h>

/*
==================
R_InitParticleTexture
==================
*/
byte	dottexture[8][8] =
{
	{0,0,0,0,0,0,0,0},
	{0,0,1,1,0,0,0,0},
	{0,1,1,1,1,0,0,0},
	{0,1,1,1,1,0,0,0},
	{0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
};

void R_InitParticleTexture(void)
{
	int		x, y;
	byte	data[8][8][4];

	//
	// particle texture
	//
	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 8; y++)
		{
			data[y][x][0] = 255;
			data[y][x][1] = 255;
			data[y][x][2] = 255;
			data[y][x][3] = dottexture[x][y] * 255;
		}
	}
	r_particletexture = DX_LoadPic("***particle***", (byte*)data, 8, 8, it_sprite, 32);

	//
	// also use this for bad textures, but without alpha
	//
	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 8; y++)
		{
			data[y][x][0] = dottexture[x & 3][y & 3] * 255;
			data[y][x][1] = 0; // dottexture[x&3][y&3]*255;
			data[y][x][2] = 0; //dottexture[x&3][y&3]*255;
			data[y][x][3] = 255;
		}
	}
	r_notexture = DX_LoadPic("***r_notexture***", (byte*)data, 8, 8, it_wall, 32);
}


/*
==============================================================================

						SCREEN SHOTS

==============================================================================
*/

typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;


/*
==================
GL_ScreenShot_f
==================
*/
void GL_ScreenShot_f(void)
{
	byte* buffer;
	char		picname[80];
	char		checkname[MAX_OSPATH];
	int			i, c, temp;
	FILE* f;

	// create the scrnshots directory if it doesn't exist
	Com_sprintf(checkname, sizeof(checkname), "%s/scrnshot", ri.FS_Gamedir());
	Sys_Mkdir(checkname);

	// 
	// find a file name to save it to 
	// 
	strcpy(picname, "quake00.tga");

	for (i = 0; i <= 99; i++)
	{
		picname[5] = i / 10 + '0';
		picname[6] = i % 10 + '0';
		Com_sprintf(checkname, sizeof(checkname), "%s/scrnshot/%s", ri.FS_Gamedir(), picname);
		f = fopen(checkname, "rb");
		if (!f)
			break;	// file doesn't exist
		fclose(f);
	}
	if (i == 100)
	{
		ri.Con_Printf(PRINT_ALL, "SCR_ScreenShot_f: Couldn't create a file\n");
		return;
	}


	buffer = (byte*)malloc(vid.width * vid.height * 3 + 18);
	memset(buffer, 0, 18);
	buffer[2] = 2;		// uncompressed type
	buffer[12] = vid.width & 255;
	buffer[13] = vid.width >> 8;
	buffer[14] = vid.height & 255;
	buffer[15] = vid.height >> 8;
	buffer[16] = 24;	// pixel size

	//qglReadPixels(0, 0, vid.width, vid.height, GL_RGB, GL_UNSIGNED_BYTE, buffer + 18);

	// swap rgb to bgr
	c = 18 + vid.width * vid.height * 3;
	for (i = 18; i < c; i += 3)
	{
		temp = buffer[i];
		buffer[i] = buffer[i + 2];
		buffer[i + 2] = temp;
	}

	f = fopen(checkname, "wb");
	fwrite(buffer, 1, c, f);
	fclose(f);

	free(buffer);
	ri.Con_Printf(PRINT_ALL, "Wrote %s\n", picname);
}

/*
** GL_Strings_f
*/
void GL_Strings_f(void)
{
	//ri.Con_Printf(PRINT_ALL, "GL_VENDOR: %s\n", dx11_config.vendor_string);
	//ri.Con_Printf(PRINT_ALL, "GL_RENDERER: %s\n", dx11_config.renderer_string);
	//ri.Con_Printf(PRINT_ALL, "GL_VERSION: %s\n", dx11_config.version_string);
	//ri.Con_Printf(PRINT_ALL, "GL_EXTENSIONS: %s\n", dx11_config.extensions_string);
}

// Эти функции нужны чтобы установить дефолтное состояние рендера, возможно их тоже придётся переписать
// Но мне кажется что не придётся, пусть в таком пустом виде останутся, чтобы не потерять

/*
** GL_SetDefaultState
*/
void GL_SetDefaultState(void)
{
	
}

void GL_UpdateSwapInterval(void)
{
	
}