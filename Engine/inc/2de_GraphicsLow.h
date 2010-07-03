#ifndef _2DE_GRAPHICS_LOW_H_
#define _2DE_GRAPHICS_LOW_H_

#include <stdarg.h>
#include <SDL/SDL.h>
#include "2de_Core.h"

#ifdef USE_SDL_OPENGL
	#include <SDL/SDL_opengl.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include "2de_ImageUtils.h"
#include "2de_MathUtils.h"

const Vector2 V2_QuadBin[4] = // Four vectors representing the quad with vertices (0 0) (1 0) (1 1) (0 1)
{
	V2_ZERO,
	V2_DIR_RIGHT,
	V2_DIR_RIGHT + V2_DIR_UP,
	V2_DIR_UP,
};

const Vector2 V2_QuadBinCenter[4] = // Four vectors representing the quad with vertices (-1 -1) (1 -1) (1 1) (-1 1)
{
	V2_DIR_LEFT + V2_DIR_DOWN,
	V2_DIR_RIGHT + V2_DIR_DOWN,
	V2_DIR_RIGHT + V2_DIR_UP,
	V2_DIR_LEFT + V2_DIR_UP,
};


const float ROTATIONAL_AXIS_Z = 1.0f;

const RGBAf COLOR_WHITE = RGBAf(1.00f, 1.00f, 1.00f, 1.00f);
const RGBAf COLOR_BLACK = RGBAf(0.00f, 0.00f, 0.00f, 1.00f);
const RGBAf COLOR_RED	= RGBAf(0.98f, 0.05f, 0.01f, 1.00f);
const RGBAf COLOR_GREEN	= RGBAf(0.10f, 0.90f, 0.05f, 1.00f);
const RGBAf COLOR_BLUE	= RGBAf(0.01f, 0.15f, 0.85f, 1.00f);


//////////////////////////////////////////////////////////////////////////
//RenderObject

/**
*	Виртуально унаследовать от этой гадости - это первый вариант.
*	Второй вариант - аггрегировать указатель на неё в классы.
*/

/*
class CPlacing 
{
	float Angle;
	Vector2 Position;
	float Scaling;
	CAABB aabb;
};
*/

class CRenderObject : public virtual CObject
{
private:
//protected:
	float				Angle;		//	(Degrees)
	float				Depth;				//	[-1; 1]?
protected:
	virtual				~CRenderObject();
public:
	Vector2				Position;		
	float				Scaling;
	CAABB				aabb;				//	Axis Aligned Bounding Box
	RGBAf				Color;
	bool				Visible;
	bool				doIgnoreCamera;

	CRenderObject();
	virtual void Render() = 0;
	void SetAngle(float AAngle = 0.0f);
	float GetAngle() const;
	void SetLayer(size_t Layer);
	float GetDepth();
};

//////////////////////////////////////////////////////////////////////////
//CGLImageData

class CGLImageData : public CImageData
{
private:
	bool			MakeTexture();
public:
					CGLImageData();
					virtual ~CGLImageData();
	bool			LoadTexture(const string &Filename);
	bool			LoadTexture(size_t AWidth, size_t AHeight, const byte* Address);	// From memory
	virtual GLuint	GetTexID();
protected:
	GLuint			TexID;
};

//////////////////////////////////////////////////////////////////////////
//CTexture

class CTexture : public CGLImageData, public CResource
{
public:
	CTexture();
	virtual ~CTexture();
	//CTexture(const string &AFilename);
	void Bind();
	bool LoadFromFile();
protected:
	void Unload();
private:
	GLuint GetTexID();
};

//////////////////////////////////////////////////////////////////////////
//CTextureManager

class CTextureManager : public CCommonManager <list <CTexture*> >/*public CList*/, public CTSingleton <CTextureManager> 
{
protected:
	CTextureManager();
	friend class CTSingleton <CTextureManager>;
};

//////////////////////////////////////////////////////////////////////////
//CFont

#define CFONT_DEFAULT_DISTANCE	1
#define CFONT_VALIGN_TOP		0x00
#define CFONT_VALIGN_BOTTOM		0x01
#define CFONT_VALIGN_CENTER		0x02
#define CFONT_VALIGN_MASK		0x0f
#define CFONT_HALIGN_LEFT		0x00
#define CFONT_HALIGN_RIGHT		0x10
#define CFONT_HALIGN_CENTER		0x20
#define CFONT_HALIGN_MASK		0xf0


#define CFONT_DEPTH_HIGH			1.0f
#define CFONT_DEPTH_LOW				0.0f
#define CFONT_MAX_STRING_LENGTH		256
#define CFONT_MAX_SYMBOLS			256

class CFont : public CResource
{
public:
	CFont();
	~CFont();

	RGBAf				tClr;						//	Цвет. Или указатель на цвет? Указатель!!! Или цвет? Блядь... ДА!
	//	Пусть будут и цвет и указатель, причём по дефолту указатель будет указывать на этот цвет. Но добавить ф-ю реэссайн колор
	RGBAf*				ptClr;						//	Вот он и есть указатель
	float				Distance;					//	Расстояние между символами		
	Vector2				Pos;						//	координты текста, для присваивания, указатель по дефолту указывает на них
	CAABB				aabb;						//	Вектор с шириной и высотой чего-то. Это для боксов. x - w, y - h
	int					offset;						//	Смещение, с которого надо выводить в боксе, если мы выводим со смещением
	int					Sel0, Sel1;					//	Номера первого и последнего символов выделенного текста
	bool				doRenderToRect;				//	Надо ли выводить текст в прямоугольник
	bool				isTextSelected;				//	Выделен ли кусок текста
	bool				doUseGlobalCoordSystem;		//	Использовать ли для вывода глобальную систему коодинат	
//	CPrimitiveRender	*PRender;					//	Настройки линий и прочей хуеты связаной с рамкой.
	int					width[CFONT_MAX_SYMBOLS];	//	Ширина символа
	int					height[CFONT_MAX_SYMBOLS];	//	Высота символа
	CRecti				bbox[256];		// Баундинг бокс каждого для каждого символа
	Vector2				scale;

	bool		LoadFromFile();
	bool		SaveToFile();

	void		Print(const char *text, ...);

	void		SetDepth(float ADepth);
	void		PointTo(Vector2 *_p);
	void		PointBack();
	void		SetAlign(const byte _Halign, const byte _Valign);

	int			GetStringWidth(const char *text);
	int			GetStringWidthEx(int t1, int t2, const char *text);
	int			GetStringHeight(const char *text);
	int			GetStringHeightEx(int t1, int t2, const char *text);
	int			StringCoordToCursorPos(const char *text, int x, int y);
	CTexture*	GetTexture();
	void		SetTexture(const string &TextureName);
	CAABB		GetSymbolsBBOX()
	{
		CAABB Result;
		Result.vMin = Vector2(9999.0f, 9999.0f);
		Result.vMax = Vector2(0.0f, 0.0f);
		for(int i = 0; i < 256; i++)
		{
			if (bbox[i].x0 < Result.vMin.x)
				Result.vMin.x = bbox[i].x0;
			if (bbox[i].y0 < Result.vMin.y)
				Result.vMin.y = bbox[i].y0;
			if (bbox[i].x1 > Result.vMax.x)
				Result.vMax.x = bbox[i].x1;
			if (bbox[i].y1 > Result.vMax.y)
				Result.vMax.y = bbox[i].y1;		
		}
		return Result;
	}

private:
	float				x, y;					//	Фактические координаты для отрисовки в _Print
	float				Depth;					//	Глубина по Z
	Vector2				*pp;					//	Указатель на вектор с координатами текста
	byte				align;					//	Флаги выравнивания
	CTexture*			Texture;				//	Указатель на текстуру шрфита. Очевидно же, да?
	GLuint				base;					//	Base List of 256 glLists for font

	void		_Print(const byte *text);
	byte		GetHalign();
	byte		GetValign();
};

class CRenderProxy : public CRenderObject
{
private:
	CRenderObject *RenderSource;

public:
	CRenderProxy(CRenderObject *ARenderSource) : RenderSource(ARenderSource)
	{
		SetName("CRenderProxy");
	}

	void Render()
	{
		RenderSource->Render();
	}
};

//////////////////////////////////////////////////////////////////////////
//Camera

class CCamera : public CObject
{
public:
	Vector2 Point;					//
	CAABB	view, outer, world;		//
	bool	Assigned;
	float	*Atx, *Aty;
	float	dx, dy;
	int		w, h;
	Vector2 p, v;

	CCamera();
	void SetWidthAndHeight(int AWidth, int AHeight);
	void Assign(float *x, float *y);
	void Free();
	void gTranslate();
	void DrawDebug();
	void Update();
};


class CFontManager : public CCommonManager <list <CFont*> >, public CTSingleton <CFontManager>
{
public:
	CFont* Font();
	bool SetCurrentFont(const char* fontname);
	bool PrintEx(int x, int y, float depth, char* text, ...);
	bool Print(int x, int y, float depth, const string &text);
	CFont* GetFont(const char* fontname);
	CFont* GetFontEx(string fontname);
	bool AddFont(CFont *AObject);

protected:
	CFont *CurrentFont;
	CFontManager();
	friend class CTSingleton<CFontManager>;
};

class CRenderManager : public CCommonManager <list <CRenderObject*> >/*public CList*/, public CTSingleton <CRenderManager>
{
protected:
	CRenderManager();
	friend class CTSingleton <CRenderManager>;
private:
	Vector2	*v2Dots, *v2Lines, *v2Quads, *v2Triangles;

public:
	CCamera	Camera;
	~CRenderManager();

	void Init()
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		// GL_COLOR_ARRAY
		// GL_TEXTURE_COORD_ARRAY
		// --
		// glVertexPointer();
		// glColorPointer();
		// glTexCoordPointer();
		// --
		//glArrayElement();
		//glArrayElementEXT()
	}

// 	void SortByAlpha();
// 	void SortByZ();
	bool DrawObjects();
//	bool PushVertex(int type, Vector2 *that);
};

//////////////////////////////////////////////////////////////////////////
// OpenGl interface// Херня какая-то передумать и переделать нахуй...

void SDLGLExit(int code);
void gSetBlendingMode();
void gBeginFrame();
void gEndFrame();

#if defined(_WIN32)
#define SWAP_INTERVAL_PROC PFNWGLSWAPINTERVALFARPROC 
#elif defined(__linux)
#define SWAP_INTERVAL_PROC PFNGLXSWAPINTERVALSGIPROC
#endif

typedef int (APIENTRY *SWAP_INTERVAL_PROC)(int);

void gToggleScissor(bool State);
void gScissor(int x, int y, int width, int height);

void setVSync(int interval=1);

//////////////////////////////////////////////////////////////////////////
//CGLWindow

class CGLWindow : public CTSingleton<CGLWindow>
{
public:
	int width;
	int height;
	byte bpp;
	bool fullscreen;
	char *caption;

	bool gCreateWindow(int _width, int _height, byte _bpp, char* _caption);
	bool gCreateWindow();
	void SetSize();
	bool glInitSystem();
	void glSuicide();
	void glResize(GLsizei Width, GLsizei Height);
	void glInit(GLsizei Width, GLsizei Height);
protected:
	CGLWindow();
	friend class CTSingleton<CGLWindow>;

};


#endif // _2DE_GRAPHICS_LOW_H_
