#include "dx11_local.h"

image_t* draw_chars;

extern	qboolean	scrap_dirty;
void Scrap_Upload(void);

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

	if (scrap_dirty)
		Scrap_Upload();

	/*GL_Bind(gl->texnum);
	qglBegin(GL_QUADS);
	qglTexCoord2f(gl->sl, gl->tl);
	qglVertex2f(x, y);
	qglTexCoord2f(gl->sh, gl->tl);
	qglVertex2f(x + w, y);
	qglTexCoord2f(gl->sh, gl->th);
	qglVertex2f(x + w, y + h);
	qglTexCoord2f(gl->sl, gl->th);
	qglVertex2f(x, y + h);
	qglEnd();*/
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
	if (scrap_dirty)
		Scrap_Upload();

	//dx11App.AddPictureToArray(x, y, gl->width, gl->height);

	/*GL_Bind(gl->texnum);
	qglBegin(GL_QUADS);
	qglTexCoord2f(gl->sl, gl->tl);
	qglVertex2f(x, y);
	qglTexCoord2f(gl->sh, gl->tl);
	qglVertex2f(x + gl->width, y);
	qglTexCoord2f(gl->sh, gl->th);
	qglVertex2f(x + gl->width, y + gl->height);
	qglTexCoord2f(gl->sl, gl->th);
	qglVertex2f(x, y + gl->height);
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