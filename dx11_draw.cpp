#include <dx11_local.h>

image_t* draw_chars;

/*
===============
Draw_InitLocal
===============
*/
void Draw_InitLocal(void)
{
	// load console characters (don't bilerp characters)
	draw_chars = DX11_FindImage("pics/conchars.pcx", it_pic);
	//GL_Bind(draw_chars->texnum);
	//qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
	int				row, col;
	float			frow, fcol, size;

	num &= 255;

	if ((num & 127) == 32)
		return;		// space

	if (y <= -8)
		return;			// totally off screen

	row = num >> 4;
	col = num & 15;

	frow = row * 0.0625;
	fcol = col * 0.0625;
	size = 0.0625;

	using namespace DirectX;
	ConstantBufferQuad cbq;
	cbq.position_transform *= XMMatrixTranspose(XMMatrixScaling(8, 8, 0) * XMMatrixTranslation(x, y, 0) * renderer->GetOrthogonal());
	cbq.texture_transform *= XMMatrixTranspose(XMMatrixScaling(size, size, 0) * XMMatrixTranslation(fcol, frow, 0));
	cbq.color[0] = 1.0f;
	cbq.color[1] = 1.0f;
	cbq.color[2] = 1.0f;
	cbq.color[3] = 1.0f;

	ui_renderer->Add(QuadDefinitions{ cbq, UI_TEXTURED, draw_chars->texnum });
}

/*
=============
Draw_FindPic
=============
*/
image_t* Draw_FindPic(char* name)
{
	image_t* gl = nullptr;
	char	fullname[MAX_QPATH];

	if (name[0] != '/' && name[0] != '\\')
	{
		Com_sprintf(fullname, sizeof(fullname), "pics/%s.pcx", name);
		gl = DX11_FindImage(fullname, it_pic);
	}
	else
		gl = DX11_FindImage(name + 1, it_pic);

	return gl;
}

/*
=============
Draw_GetPicSize
=============
*/
void Draw_GetPicSize(int* w, int* h, char* pic)
{
	image_t* gl;

	gl = Draw_FindPic(pic);
	if (!gl)
	{
		*w = *h = -1;
		return;
	}
	*w = gl->width;
	*h = gl->height;
}

/*
=============
Draw_StretchPic
=============
*/
void Draw_StretchPic(int x, int y, int w, int h, char* pic)
{
	image_t* gl;

	gl = Draw_FindPic(pic);
	if (!gl)
	{
		ri.Con_Printf(PRINT_ALL, "Can't find pic: %s\n", pic);
		return;
	}

	using namespace DirectX;
	ConstantBufferQuad cbq;
	cbq.position_transform *= XMMatrixTranspose(XMMatrixScaling(w, h, 0) * XMMatrixTranslation(x, y, 0) * renderer->GetOrthogonal());
	cbq.color[0] = 1.0f;
	cbq.color[1] = 1.0f;
	cbq.color[2] = 1.0f;
	cbq.color[3] = 1.0f;

	ui_renderer->Add(QuadDefinitions{ cbq, UI_TEXTURED, gl->texnum });
}


/*
=============
Draw_Pic
=============
*/
void Draw_Pic(int x, int y, char* pic)
{
	image_t* gl;

	gl = Draw_FindPic(pic);
	if (!gl)
	{
		ri.Con_Printf(PRINT_ALL, "Can't find pic: %s\n", pic);
		return;
	}

	using namespace DirectX;
	ConstantBufferQuad cbq;
	cbq.position_transform *= XMMatrixTranspose(XMMatrixScaling(gl->width, gl->height, 0) * XMMatrixTranslation(x, y, 0) * renderer->GetOrthogonal());
	cbq.color[0] = 1.0f;
	cbq.color[1] = 1.0f;
	cbq.color[2] = 1.0f;
	cbq.color[3] = 1.0f;

	ui_renderer->Add(QuadDefinitions{ cbq, UI_TEXTURED, gl->texnum });
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
	image_t* image;

	image = Draw_FindPic(pic);
	if (!image)
	{
		ri.Con_Printf(PRINT_ALL, "Can't find pic: %s\n", pic);
		return;
	}

	using namespace DirectX;
	ConstantBufferQuad cbq;
	cbq.position_transform *= XMMatrixTranspose(XMMatrixTranslation(x, y, 0) * XMMatrixScaling(w, h, 0) * renderer->GetOrthogonal());
	cbq.texture_transform *= XMMatrixTranspose(XMMatrixScaling(w / 64.0f, h / 64.0f, 0) * XMMatrixTranslation(x / 64.0f, y / 64.0f, 0));
	cbq.color[0] = 1.0f;
	cbq.color[1] = 1.0f;
	cbq.color[2] = 1.0f;
	cbq.color[3] = 1.0f;

	ui_renderer->Add(QuadDefinitions{ cbq, UI_TEXTURED, image->texnum });
}

/*
=============
Draw_Fill

Fills a box of pixels with a single color
=============
*/
void Draw_Fill(int x, int y, int w, int h, int c)
{
	union
	{
		unsigned	c;
		byte		v[4];
	} color;

	if ((unsigned)c > 255)
		ri.Sys_Error(ERR_FATAL, "Draw_Fill: bad color");

	color.c = d_8to24table[c];

	using namespace DirectX;
	ConstantBufferQuad cbq;
	cbq.position_transform *= XMMatrixTranspose(XMMatrixTranslation(x, y, 0) * XMMatrixScaling(w, h, 0) * renderer->GetOrthogonal());
	cbq.color[0] = color.v[0] / 255;
	cbq.color[1] = color.v[1] / 255;
	cbq.color[2] = color.v[2] / 255;
	cbq.color[3] = 1.0f;

	ui_renderer->Add(QuadDefinitions{ cbq, UI_COLORED, -1 });
}

/*
================
Draw_FadeScreen

================
*/
void Draw_FadeScreen(void)
{
	using namespace DirectX;
	ConstantBufferQuad cbq;
	cbq.position_transform *= XMMatrixTranspose(XMMatrixScaling(vid.width, vid.height, 0) * renderer->GetOrthogonal());
	cbq.color[0] = 0.0f;
	cbq.color[1] = 0.0f;
	cbq.color[2] = 0.0f;
	cbq.color[3] = 0.8f;

	ui_renderer->Add(QuadDefinitions{ cbq, UI_COLORED, -1 });
}

/*
=============
Draw_StretchRaw
=============
*/
extern unsigned	r_rawpalette[256];
bool firstLoading = true;

void Draw_StretchRaw(int x, int y, int w, int h, int cols, int rows, byte* data)
{
	unsigned	image32[256 * 256];
	unsigned char image8[256 * 256];
	int			i, j, trows;
	byte* source;
	int			frac, fracstep;
	float		hscale;
	int			row;
	float		t;

	if (rows <= 256)
	{
		hscale = 1;
		trows = rows;
	}
	else
	{
		hscale = rows / 256.0;
		trows = 256;
	}
	t = rows * hscale / 256;

	unsigned* dest;

	for (i = 0; i < trows; i++)
	{
		row = (int)(i * hscale);
		if (row > rows)
			break;
		source = data + cols * row;
		dest = &image32[i * 256];
		fracstep = cols * 0x10000 / 256;
		frac = fracstep >> 1;
		for (j = 0; j < 256; j++)
		{
			dest[j] = r_rawpalette[source[frac >> 16]];
			frac += fracstep;
		}
	}

	if (firstLoading)
	{
		renderer->AddTexturetoSRV(256, 256, 32, (unsigned char*)image32, CINEMATIC_PICTURE, false);
		firstLoading = false;
	}
	else
	{
		renderer->UpdateTextureInSRV(256, 256, 0, 0, 32, (unsigned char*)image32, CINEMATIC_PICTURE);
	}

	using namespace DirectX;
	ConstantBufferQuad cbq;
	cbq.position_transform *= XMMatrixTranspose((XMMatrixTranslation(x, y, 0) * XMMatrixScaling(w, h, 0)) * renderer->GetOrthogonal());
	cbq.texture_transform *= XMMatrixTranspose(XMMatrixScaling(1.0f, t, 0));
	cbq.color[0] = 1.0f;
	cbq.color[1] = 1.0f;
	cbq.color[2] = 1.0f;
	cbq.color[3] = 1.0f;

	ui_renderer->Add(QuadDefinitions{ cbq, UI_TEXTURED, CINEMATIC_PICTURE });
}