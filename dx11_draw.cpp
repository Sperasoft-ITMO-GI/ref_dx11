#include "dx11_local.h"

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

	Vertex_PosTexCol_Info info = {};
	info.pos.x = x;
	info.pos.y = y;
	info.pos.width = 8;
	info.pos.height = 8;

	info.tex.xl = fcol;
	info.tex.xr = fcol + size;
	info.tex.yt = frow;
	info.tex.yb = frow + size;

	info.col.x = 1.0;
	info.col.y = 1.0;
	info.col.z = 1.0;
	info.col.w = 1.0;

	dx11App.DummyDrawingPicture(&info, draw_chars->texnum);

	/*GL_Bind(draw_chars->texnum);

	qglBegin(GL_QUADS);
	qglTexCoord2f(fcol, frow);
	qglVertex2f(x, y);
	qglTexCoord2f(fcol + size, frow);
	qglVertex2f(x + 8, y);
	qglTexCoord2f(fcol + size, frow + size);
	qglVertex2f(x + 8, y + 8);
	qglTexCoord2f(fcol, frow + size);
	qglVertex2f(x, y + 8);
	qglEnd();*/
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

	// TODO: check for alpha test if image->hax_alpha then enable it

	Vertex_PosTexCol_Info info = {};
	info.pos.x = x;
	info.pos.y = y;
	info.pos.width = gl->width;
	info.pos.height = gl->height;

	info.tex.xl = 0.0f;
	info.tex.xr = 1.0f;
	info.tex.yt = 0.0f;
	info.tex.yb = 1.0f;

	info.col.x = 1.0;
	info.col.y = 1.0;
	info.col.z = 1.0;
	info.col.w = 1.0;

	dx11App.DummyDrawingPicture(&info, gl->texnum);
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

	// TODO: check for alpha test if image->hax_alpha then enable it

	Vertex_PosTexCol_Info info = {};
	info.pos.x = x;
	info.pos.y = y;
	info.pos.width = gl->width;
	info.pos.height = gl->height;

	info.tex.xl = 0.0f;
	info.tex.xr = 1.0f;
	info.tex.yt = 0.0f;
	info.tex.yb = 1.0f;

	info.col.x = 1.0;
	info.col.y = 1.0;
	info.col.z = 1.0;
	info.col.w = 1.0;

	dx11App.DummyDrawingPicture(&info, gl->texnum);
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

	// TODO: check for alpha test if image->hax_alpha then enable it

	Vertex_PosTexCol_Info info = {};
	info.pos.x = x;
	info.pos.y = y;
	info.pos.width = w;
	info.pos.height = h;

	info.tex.xl = x / 64.0f;
	info.tex.xr = (x + w) / 64.0f;
	info.tex.yt = y / 64.0f;
	info.tex.yb = (y + h) / 64.0f;

	info.col.x = 1.0;
	info.col.y = 1.0;
	info.col.z = 1.0;
	info.col.w = 1.0;

	dx11App.DummyDrawingPicture(&info, image->texnum);

	/*GL_Bind(image->texnum);
	qglBegin(GL_QUADS);
	qglTexCoord2f(x / 64.0, y / 64.0);
	qglVertex2f(x, y);
	qglTexCoord2f((x + w) / 64.0, y / 64.0);
	qglVertex2f(x + w, y);
	qglTexCoord2f((x + w) / 64.0, (y + h) / 64.0);
	qglVertex2f(x + w, y + h);
	qglTexCoord2f(x / 64.0, (y + h) / 64.0);
	qglVertex2f(x, y + h);
	qglEnd();*/
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

	std::array<std::pair<float, float>, 4> pos = {
		std::make_pair(x, y),
		std::make_pair(x + w, y),
		std::make_pair(x, y + h),
		std::make_pair(x + w, y + h),
	};

	std::array<float, 4> col = { color.v[0] / 255, color.v[1] / 255, color.v[2] / 255, 1.0f };

	dx11App.DrawColored2D(pos, col);
}

/*
================
Draw_FadeScreen

================
*/
void Draw_FadeScreen(void)
{
	std::array<std::pair<float, float>, 4> pos = {
		std::make_pair(0.0f, 0.0f),
		std::make_pair(vid.width, 0.0f),
		std::make_pair(0.0f, vid.height),
		std::make_pair(vid.width, vid.height),
	};

	std::array<float, 4> col = { 0.0f, 0.0f, 0.0f, 0.8f };

	dx11App.DrawColored2D(pos, col);
}

/*
=============
Draw_StretchRaw
=============
*/
extern unsigned	r_rawpalette[256];

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

	//dx11App.DummyTest("pics/test.pcx", 256, 256, 32, (unsigned char*)image32, 1);

	dx11App.AddTexturetoSRV(256, 256, 32, (unsigned char*)image32, 1300);

	//qglTexImage2D(GL_TEXTURE_2D, 0, gl_tex_solid_format, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, image32);

	//qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// TODO: ENABLE ALPHA TEST

	Vertex_PosTexCol_Info info = {};
	info.pos.x = x;
	info.pos.y = y;
	info.pos.width = w;
	info.pos.height = h;

	info.tex.xl = 0.0f;
	info.tex.xr = 1.0f;
	info.tex.yt = 0.0f;
	info.tex.yb = t;

	info.col.x = 1.0;
	info.col.y = 1.0;
	info.col.z = 1.0;
	info.col.w = 1.0;

	dx11App.DummyDrawingPicture(&info, 1300);
}