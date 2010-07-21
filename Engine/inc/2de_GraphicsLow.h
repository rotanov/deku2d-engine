#ifndef _2DE_GRAPHICS_LOW_H_
#define _2DE_GRAPHICS_LOW_H_

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

extern const unsigned int BINARY_DATA_DEFAULT_FONT_SIZE;
extern char BINARY_DATA_DEFAULT_FONT[];

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
	float				Angle;		//	(Degrees)
	CAABB				aabb;				//	Axis Aligned Bounding Box; ORLY? Isn't it obvious
	float				Depth;		//	[-1; 1]?		
	float				Scaling;	

public:
	Vector2				Position;		
	RGBAf				Color;
	bool				Visible;
	bool				doIgnoreCamera;		// if true, then object will be drawn in global coords, no matter where camera pointing;

	CRenderObject();
	virtual ~CRenderObject();
	virtual void Render() = 0;
	void SetAngle(float AAngle = 0.0f);
	float GetAngle() const;
	const CAABB& GetBox() const;
	virtual void SetBox(const CAABB &box);
	float GetScaling() const;
	void SetScaling(float AScaling);
	void SetLayer(size_t Layer);
	float GetDepth() const;
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

class CTextureManager : public CCommonManager <list <CTexture*> >, public CTSingleton <CTextureManager> 
{
protected:
	CTextureManager();
	friend class CTSingleton <CTextureManager>;
};

//////////////////////////////////////////////////////////////////////////
//CFont

#define CFONT_DEFAULT_DISTANCE	1


#define CFONT_MAX_STRING_LENGTH		256
#define CFONT_MAX_SYMBOLS			256

class CFont : public CResource
{
	friend class CRenderManager;
public:
	CRecti				bbox[256];		// Баундинг бокс каждого для каждого символа
	// CRecti - что за хуита - это тип только здесь используется. Нахуй нам инт, когда float is fine too;
	// Временно в паблике, так как редактор шрифтов использует CFont напрямую и устанавливает ему параметры так же напрямую.
	// А мне лень переписывать сейчас редактор шрифтов

	CFont();
	~CFont();
	bool		LoadFromFile();
	bool		SaveToFile();
	bool		LoadFromMemory(const byte* Address);

	void		Print(const char *text);


	int			GetStringWidth(const string &text);
	int			GetStringWidthEx(int t1, int t2, const string &text);
	int			GetStringHeight(const string &text);
	int			GetStringHeightEx(int t1, int t2, const string &text);
	int			StringCoordToCursorPos(const string &text, int x, int y);
	CTexture*	GetTexture();
	void		SetTexture(const string &TextureName);
	CAABB		GetSymbolsBBOX();

private:
	float				Distance;					//	Расстояние между символами		
	int					width[CFONT_MAX_SYMBOLS];	//	Ширина символа
	int					height[CFONT_MAX_SYMBOLS];	//	Высота символа
	
	CTexture*			Texture;				//	Указатель на текстуру шрфита. Очевидно же, да?
	GLuint				base;					//	Base List of 256 glLists for font

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
private:
	CFont *DefaultFont;
	CFont *CurrentFont;

protected:	
	CFontManager();
	friend class CTSingleton<CFontManager>;

public:
	void Init();
	CFont* Font(); // @todo: rename
	CFont* GetDefaultFont()
	{
		assert(DefaultFont != NULL);
		return DefaultFont;
	}
	bool SetCurrentFont(const char* fontname);
	CFont* GetFont(const string &fontname);	
	bool AddFont(CFont *AObject); // WTF>?
};

class CText;

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
		// blahblhablha my personal OpenGL Sandbox here; Sorry();
		//glEnableClientState(GL_VERTEX_ARRAY);
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
//	bool PushVertex(int type, Vector2 *that);
	bool DrawObjects();
	void Print(const CText *Text) const;
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
//CGLWindow // too old

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
//////////////////////////////////////////////////////////////////////////
//CText
class CText : public CRenderObject
{
private:
	CFont *Font;
	string Text;

public:
	CText();
	~CText()
	{
		// this is destructor. i can set breakpoint there
	}
	CText(const string &AText) : Text(AText), Font(CFontManager::Instance()->GetDefaultFont())
	{
		assert(Font != NULL);
		doIgnoreCamera = true;
	}
	void Render();
	CFont* GetFont() const;
	string& GetText();
	const string& GetText() const;
	void SetFont(CFont *AFont);
	void SetText(const string &AText);
	int Height();	// @todo: вынести Height и Width в переменные и обновлять при изменении текста.
	int Width();
	CText& operator =(const string &AText);
	string operator +(const CText &Text) const;
	string operator +(const char *Text) const;
};

#endif // _2DE_GRAPHICS_LOW_H_
