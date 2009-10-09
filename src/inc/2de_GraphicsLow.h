#ifndef _GRAPHICS_LOW_H_
#define _GRAPHICS_LOW_H_

#include <stdarg.h>
#include <SDL.h>
#include "2de_Core.h"

#ifdef USE_SDL_OPENGL
#include "SDL_opengl.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "2de_ImageUtils.h"
#include "2de_MathUtils.h"

const Vector2 V2_QuadBin[4] = 
{
	V2_Z,
	V2_DIR_RIGHT,
	V2_DIR_RIGHT + V2_DIR_UP,
	V2_DIR_UP
};

//////////////////////////////////////////////////////////////////////////
//RenderObject

class CRenderObject : public virtual CObject
{
public:
	union									//	Здесь немного усложнил c union, но думаю, так будет удобнее пользоваться. 
	{										
		struct								
		{
			Vector2	position;
			scalar	depth;
		};
		struct  
		{
			scalar	x, y, z;
		};
// 		struct								//	И, кстати, что-то меня тянет также добавить возможность обращаться к координатам и как к	
// 		{									//	Vector3
// 			Vector3	Position;				
// 		};
	};
	CAABB				aabb;				// Axis Aligned Bounding Box
	bool				visible;
	RGBAf				color;
	CRenderObject();	
	virtual bool Render() = 0;
	virtual ~CRenderObject();
};

//////////////////////////////////////////////////////////////////////////
//CGLImageData

class CGLImageData : public CImageData
{
public:
	bool	CleanData;
					CGLImageData();
					~CGLImageData();
	bool			MakeTexture();
	bool			LoadTexture(const char *filename);
	virtual GLuint	GetTexID();
protected:
	GLuint TexID;
};

//////////////////////////////////////////////////////////////////////////
//CTexture

class CTexture : public CGLImageData, public CResource
{
public:
	CTexture();
	~CTexture();
	CTexture(char * vfilename);
	static CObject* NewTexture()
	{
		return new CTexture;
	}
	void Bind();
	bool LoadFromFile();
protected:
	void Unload();
private:
	GLuint GetTexID();
};

//////////////////////////////////////////////////////////////////////////
//CTextureManager

class CTextureManager : public CList
{
public:
	CTexture* GetTextureByName(const string &TextureName);
	static CTextureManager* Instance();
protected:
	CTextureManager();
	~CTextureManager();
	static CTextureManager * _instance;
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
	scalar				Distance;					//	Расстояние между символами		
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
	CRecti		bbox[256];		// Баундинг бокс каждого для каждого символа
	Vector2				scale;

	static CObject* NewFont()
	{
		return new CFont;
	}
	bool		LoadFromFile();
	bool		SaveToFile();

	void		Print(const char *text, ...);

	void		SetDepth(float ADepth);
	void		AssignTexture(CTexture* AFontTexture);
	void		PointTo(const Vector2 *_p);
	void		PointBack();
	void		SetAlign(const byte _Halign, const byte _Valign);

	int			GetStringWidth(const char *text);
	int			GetStringWidthEx(int t1, int t2, const char *text);
	int			GetStringHeight(const char *text);
	int			GetStringHeightEx(int t1, int t2, const char *text);

private:
	float				x, y;					//	Фактические координаты для отрисовки в _Print
	float				Depth;					//	Глубина по Z
	(const Vector2)		*pp;					//	Указатель на вектор с координатами текста
	byte				align;					//	Флаги выравнивания
	CTexture*			Texture;				//	Указатель на текстуру шрфита. Очевидно же, да?
	GLuint		base;			// Base List of 256 glLists for font

	void		_Print(const char *text);
	byte		GetHalign();
	byte		GetValign();
};

//////////////////////////////////////////////////////////////////////////
//Camera

class CCamera : public CObject
{
public:
	Vector2 Point;					//
	CAABB view, outer, world;		//
	bool Assigned;
	scalar *Atx, *Aty;
	scalar dx, dy;
	int w, h;
	Vector2 p, v;

	CCamera()
	{
		name = "Camera";
		view = world = CAABB(100, 100, 540, 380);
		outer = CAABB(-1024, 0, 2048, 512);
		Point = p = v = Vector2::Blank();
		Atx = Aty = NULL;
		Assigned = false;
	}

	void Assign(scalar *x, scalar *y);
	void Free();
	void gTranslate();
	void DrawDebug();
	void Update();
};


class CFontManager : public CList
{
public:
	static CFontManager*	Instance();
	CFont					*CurrentFont;
	bool					SetCurrentFont(char* fontname);
	bool					PrintEx(int x, int y, float depth, char* text, ...);
	bool					Print(int x, int y, float depth, char* text, ...);
	CFont*					GetFont(const char* fontname);
	CFont*					GetFontEx(string fontname);
	bool					AddObject(CObject *object);

protected:
	CFontManager();
	~CFontManager();
	static			CFontManager* _instance;
};

class CRenderManager : public CList
{
private:
	Vector2	*v2Dots, *v2Lines, *v2Quads, *v2Triangles;

public:
	CCamera	Camera;
	CRenderManager();
	~CRenderManager();

	void SortByAlpha();
	void SortByZ();
	bool DrawObjects();
	bool PushVertex(int type, Vector2 *that);
};

//////////////////////////////////////////////////////////////////////////
// OpenGl interface// Херня какая-то передумать и переделать нахуй...

void SDLGLExit(int code);
void gSetBlendingMode();
void gBeginFrame();
void gEndFrame();
typedef BOOL (__stdcall *PFNWGLSWAPINTERVALFARPROC)(int);
void gToggleScissor(bool State);
void gScissor(int x, int y, int width, int height);
void gSetColor(RGBAf color);

void setVSync(int interval=1);

//////////////////////////////////////////////////////////////////////////
//CGLWindow

class CGLWindow
{
public:
	int width;
	int height;
	byte bpp;
	bool fullscreen;
	char* caption;

	CGLWindow();
	~CGLWindow();
	bool gCreateWindow(int _width, int _height, byte _bpp, char* _caption);
	bool gCreateWindow();
	void SetSize();
	bool glInitSystem();
	void glSuicide();
	void glResize(GLsizei Width, GLsizei Height);
	void glInit(GLsizei Width, GLsizei Height);
};


#endif _GRAPHICS_LOW_H_
