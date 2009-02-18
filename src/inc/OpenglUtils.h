#ifndef OPENGLUTILS_H
#define OPENGLUTILS_H

#include <stdarg.h>
#include <SDL.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "CoreUtils.h"
#include "ImageUtils.h"
#include "MathUtils.h"
#define LOWBYTE(bt) (bt&0xff)
#define _GetRValue(rgb)      (LOWBYTE(rgb))
#define _GetGValue(rgb)      (LOWBYTE(((WORD)(rgb)) >> 8))
#define _GetBValue(rgb)      (LOWBYTE((rgb)>>16))

// #define _DEBUG_DISABLE_PARTICLES_DRAW
// #define _DEBUG_DISABLE_PARTICLES_UPDATE

class glRGBAub : public RGBAub
{
public:
	glRGBAub();
	glRGBAub(byte _r, byte _g, byte _b, byte _a);
	void glSet();
};

class CRenderObject : public virtual CObject
{
public:
	float				x, y, z;
	bool				visible;
	glRGBAub			color;
	CRenderObject() : x(0), y(0), z(0), visible(true), color(255, 255, 255, 255)
	{
		name += "CRenderObject ";
		type |= T_RENDERABLE;
	};
	void SetColor(byte _r, byte _g, byte _b, byte _a);
	virtual bool Render() = 0;
	virtual ~CRenderObject(){};
};




class CCamera : public CObject
{
public:
	Vector2 Point;	// point to which translation is made.
	CBBox view, outer, world;		// inner screen bbox
	bool Assigned;
	scalar *Atx, *Aty;
	scalar dx, dy;
	int w, h;
	Vector2 p, v;

	CCamera()
	{
		name = "Camera";
		view = world = CBBox(100, 100, 540, 380);
		outer = CBBox(-1024, 0, 2048, 512);
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



class CGLImageData : public CImageData
{
public:
	bool CleanData;

	/**
	*	Constructor
	*/
	CGLImageData();
	/**
	*	Make TexID member able to use as texture in BindTexture() calls.
	*/
	bool	MakeTexture();
	/**
	*	Loads texture.
	*/
	bool LoadTexture(const char *filename);
	/**
	*	Draw an whole image
	*	If you need to draw a one image
	*	more than once, it is bad to performance
	*	to use this function.
	*/
	bool RenderWhole(int zoom);
	/**
	*	Begins quad drawing.
	*/
	bool BeginDraw();
	/**
	*	Sends a quad from image to video memory.
	*/
	bool PushQuad(float x0, float y0, float z0, float _width, float _height, int s0, int t0, int s1, int t1);
	bool PushQuadEx(float x0, float y0, float z0, float _width, float _height, int s0, int t0, int s1, int t1, bool tiled);
	/**
	*	Ends Drawing procedure.
	*/
	bool EndDraw();
	virtual GLuint& GetTexID();

private:
	bool isDrawing;
protected:
	GLuint TexID;
};

class CTexture : public CGLImageData, public CResource, public CRenderObject
{
public:
	CTexture(char * vfilename)
	{
		filename = vfilename;
		name = "CTexture";
		this->type = T_RENDERABLE;
	}
	void Bind()
	{
		glBindTexture(GL_TEXTURE_2D, GetTexID());
	}
	bool Render()
	{
		return true;
	}
	CTexture()
	{
	};
	static CObject* NewTextureRes()
	{
		return new CTexture;
	}
	bool Load();
	GLuint& GetTexID();
};

class CTextureManager : public CObjectList
{
public:
	bool AddTexture(char* filename, bool load = false);
	bool LoadTextureByName(char *TextureName);
	bool LoadAllTextures();
	static CTextureManager* Instance();
	void FreeInst();
protected:
	CTextureManager();
	~CTextureManager();
	static CTextureManager * _instance;
	static int _refcount;
};



#define ANIM_NONE			-1
#define ANIM_WALK_LEFT		0x01
#define ANIM_WALK_RIGHT		0x02
#define ANIM_STAND_LEFT		0x03
#define ANIM_STAND_RIGHT	0x04
#define	ANIM_ATTAK_DEMO		0x05

struct SAnimationInfo				// структура определяющая параметры анимации
{
	bool	isAnimated;				// анимирован ли спрайт
	bool	isLoop;					// залуплена ли анимация
	float	m_fFrameDelay;			// задержка между кадрами в мс
	float	m_fwidth, m_fheight;	// реальная ширина и высота спрайта для отображения на экране
			
	int	m_nNumFrameColumns,			// кол-во колонок в анимации
		m_nNumFrameRows,			// кол-во строк в анимации
		m_nTotalFrames,				// кол-во кадров в анимации
		m_nFrameWidth,				// ширина кадра в пикселях
		m_nFrameHeight,				// высота кадра в пикселях
		m_nOffsetX,					// расстояние от левого края до первого кадра в пикселях по Х
		m_nOffsetY;					// расстояние от левого края до первого кадра в пикселях по У
	int AnimationIndex;				// индекс анимации
};

class CSprite : public CRenderObject
{
public:
	int			m_nTextureWidth,		// ширина текстуры в пикселях
				m_nTextureHeight;		// тоже, но длина
	glRGBAub	color;					// цвет
	GLuint		m_textureID;			// опенгловская хрень идентиф. текстуру

	DWORD		ellapsedtime, lasttime;	// херь чтобы делать время между кадрами
	int			m_nFrameNumber,			// номер текущего кадра
				m_nCurrentRow,			// номер текущей строки
				m_nCurrentColumn;		// номер текущей клонки

	bool		m_bFirstRendering;		// первый ли раз происходит отрисовка спрайта
	byte		numAnimations;			// кол-во анимаций для одного спрайта

	SAnimationInfo *animations;		// настройки анимаций. загрузяются из файла
	SAnimationInfo *anim;			// указатель на текущую анимацию
	bool mirror_h;
	
	/**
	*	constructor && destructor
	*/
	CSprite();
	~CSprite();
	CSprite* Instance();
	/**
	*	Load Animation Set file & use it as texture
	*	filename is filename either.
	*/
	bool LoadTexture(char * filename);
	/**
	*	OMFG. I hate doing such things. But this function
	*	is for testing and for editor.
	*	No. Really there are too much arguments.
	*/
	bool AddAnimation(bool _isAnimated, float _m_fFrameDelay, float _m_fwidth,
					float _m_fheight, int _m_nNumFrameColumns, int _m_nNumFrameRows,
					int _m_nTotalFrames, int _m_nFrameWidth, int _m_nFrameHeight,
					int _m_nOffsetX, int _m_nOffsetY, int _AnimationIndex, bool _isLoop);	
	/**
	*	function Render() draw an current frame of animation
	*	to the screen. That's all.
	*/
	bool Render();
	bool Render(float x, float y);
	/**
	*	SetAnimation() sets the current animation. For now index is just an animation
	*	index in array.
	*/
	void SetAnimation(int index);
	/**
	*	Finds address of animation with this index.
	*/
	SAnimationInfo* FindAnimation(int index);
	/**
	*	Loads an animation settings from file.
	*/
	bool LoadFromFile(char* filename);
	/**
	*	Saves animation settings to file.
	*/
	bool SaveToFile(char *filename);
};

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
	/**
	*	Initializes OpenGL routines.	
	*/
	bool glInitSystem();
	/**
	*	releases OpenGL and so on.
	*/
	void glSuicide();
	/**
	*	This function calls each time window resizes.	
	*/
	void glResize(GLsizei Width, GLsizei Height);
	/**
	*	The glInit function calls once before
	*	entering to main loop. Here are placed
	*	all OpenGL configurations.
	*/
	void glInit(GLsizei Width, GLsizei Height);
};

#define CFONT_DEFAULT_DISTANCE 1
#define CFONT_VALIGN_TOP		0x00
#define CFONT_VALIGN_BOTTOM		0x01
#define CFONT_VALIGN_CENTER		0x02
#define CFONT_VALIGN_MASK		0x0f
#define CFONT_HALIGN_LEFT		0x00
#define CFONT_HALIGN_RIGHT		0x10
#define CFONT_HALIGN_CENTER		0x20
#define CFONT_HALIGN_MASK		0xf0

class CText : public CRenderObject
{
private:
	void Draw()
	{
			
	}
//public:

};

class CFont : public CObject
{
public:
	CFont();
	~CFont();
	/**
	*	Loads font configuration from *.fif file.
	*/
	bool			LoadFromFile(char* filename);
	/**
	*	Saves font configuration to file.
	*/
	bool			SaveToFile(char* filename);
	/**
	*	Print() simply prints string at (x, y) coordinates
	*/
	void			Print(int x, int y, float depth, char *text);
	/**
	*	PrintEx() prints string at (x, y) coordinates with parameters
	*/
	void			PrintEx(int x, int y, float depth, char *text, ...);
	/**
	*	PrintRect() prints string at (x, y) from offset in text
	*	Printed text is cut within rectangle(x, y, x+width, y+height) 
	*/
	void			PrintRect(int x, int y, float depth, int width, int height, int offset, char *text);
	/**
	*	PrintRectEx() prints string at (x, y) from offset in text
	*	Printed text is cut within rectangle(x, y, x+width, y+height) 
	*	align is bitwise "or" combination of align constants like CFONT_HALIGN_CENTER
	*/
	void			PrintRectEx(int x, int y, float depth, int width, int height, int offset, byte align, char *text);
	/**
	*	PrintSelRect() prints string at (x, y) from offset in text
	*	Printed text is cut within rectangle(x, y, x+width, y+height) 
	*	align is bitwise "or" combination of align constants like CFONT_HALIGN_CENTER
	*	text is selected with currently blue rect from s1 to s2 symbol
	*/
	void			PrintSelRect(int x, int y, float depth, int width, int height, int offset, byte align, char *text, int s1, int s2);
	/**
	*	GetStringWidth() returns width of text in pixels
	*/
	int				GetStringWidth(char *text);
	/**
	*	GetStringWidthEx() returns width of substring in text
	*	from t1 to t2 char in text in Pixels
	*/
	int				GetStringWidthEx(int t1, int t2, char *text);
	/**
	*	GetStringHeight() returns height of text in pixels
	*/
	int				GetStringHeight(char *text);

private:
	CRecti		bbox[256];		// Bounding boxes for each symbol in coordinates of font texture
	byte		width[256];		// Width of Each symbol used to do something. >.>
	byte		height[256];	// Height of Each symbol used to do something. >.>
	char		*FontImageName;	// Name of the file of the font texture
	GLuint		font;			// Font texture ID
	GLuint		base;			// Base List of 256 glLists for font
	byte		dist;			// Distance between symbols
};
typedef CFont*	PFont;

class CFontManager : public CObjectList
{
public:
	static			CFontManager* Instance(char *byWho);
	void			FreeInst(char *byWho);
	CFont			*CurrentFont;
	bool			SetCurrentFont(char* fontname);
	bool			PrintEx(int x, int y, float depth, char* text, ...);
	bool			Print(int x, int y, float depth, char* text, ...);
	CFont*			GetFont(char* fontname);
	CFont*			GetFontEx(string fontname);
	bool			AddObject(CObject *object);

protected:
	CFontManager();
	~CFontManager();
	static			CFontManager* _instance;
	static int		_refcount;
};

class CRenderManager : public CObjectList
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


/**
*	Here goes particle system stuff
*/

int Random_Int(int min, int max);
float Random_Float(float min, float max);

struct CParticle
{
	Vector2			p;			// position of particle
	Vector2			v;			// velocity of particle
	RGBAf			c;			// current color
	RGBAf			dc;			// color delta
	float			size;		// current size of particle
	float			dsize;		// delta size i.e add this to size every step 0 means nothing
	float			life;		// lifetime of the particle -1 means particle is immortal
	float			age;		// current age of the particle
	float			Period;					// Yep... a ha... ha ha ha
};

struct CPsysteminfo
{
	Vector2			p;						// particle system position

	RGBAf			sc;						// desired start color of each particle
	RGBAf			ec;						// desired end color of each particle
	RGBAf			vc;						// color variability

	int				MaxParticles;			// number of possible maximum number particles in the system
	int				ParticlesActive;		// current number of active particles
	int				lifemin;				// minimum life of particle
	int				lifemax;				// maximum life of particle

	float			startsize;				// startsize of each particle
	float			endsize;				// end size
	float			sizevar;				// size variability

	float			plife;					// desired particle life. -1 is ever
	int				plifevar;				// desired particle life variability

	float			life;					// life of the system -1 means immortal
	int				age;					// current age of the system. inc only if mortal

	int				emission;				// emission means emission :3
	int				notcreated;				// the number of particle left to be created in next step for may be some reason
	Vector2			*geom;					// array of points for particle generation
	int				GeomNumPoints;			// number of them
	bool			isSnow;					// temporary
};

typedef void (*FCreateFunc)(CParticle *);
typedef void (*FUpdateFunc)(CParticle *, float);

class CParticleSystem :  public CResource, public CRenderObject, public CUpdateObject
{
public:
	CPsysteminfo			info;
	CParticle				*particles;
	GLuint					TexID;
	bool					user_create;
	bool					user_update;
	void					(*procUserCreate)(CParticle *);
	void					(*procUserUpdate)(CParticle *, float);

	void					Init();	
	bool					Update(float dt);
	bool					Render();
	CParticle				*CreateParticle();
	void					SetGeometry(Vector2 * points, int numPoints);
	void					SetUserUpdate(FUpdateFunc func);
	void					SetUserCreate(FCreateFunc func);
	
	bool					SaveToFile();
	bool					LoadFromFile();

protected:
	void					_swap(int i, int j);
};

//////////////////////////////////////////////////////////////////////////
// OpenGl interface
//////////////////////////////////////////////////////////////////////////

/**
*	Terminates the program under SDL & Opengl.
*/
void SDLGLExit(int code);
/**
*	Sets specific blending mode
*/
void gSetBlendingMode();
/**
*	Should be called before every frame
*/
void gBeginFrame();
/**
*	Should be called after each frame
*/
void gEndFrame();

#define G_PRIM_BLEND_OPT
#define G_POLY_TEX_CELL_SIZE 20
#define G_POLY_OUTLINE_ENABLE
#define G_POLY_TEXTURE_ENABLE
#define PRM_RNDR_OPT_BLEND_ONE 0x01

class CPrimitiveRender
{
public:
	int BlendingOption, CheckerCellSize;
	bool OutlineEnabled, TextureEnabled, DashedLines;
	int glListCircle, glListRing;
	Vector2 *wh;
	scalar Radius, Angle, lwidth, depth;
	int dash;
	RGBAf *lClr, *sClr, *pClr;

	CPrimitiveRender()
	{
			pClr = sClr = lClr = NULL;
			
			glListCircle = glGenLists(1);
			glNewList(glListCircle, GL_COMPILE_AND_EXECUTE);
			glBegin(GL_LINE_LOOP);
			for(int i = 0; i < 64 + 1; i ++)
			{
				Vector2 P(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
				glVertex2f(P.x, P.y);
			}
			glEnd();
			glEndList();

			glListRing = glGenLists(1);
			glNewList(glListRing, GL_COMPILE_AND_EXECUTE);
			glBegin(GL_LINE_LOOP);
			for(int i = 0; i < 64 + 1; i ++)
			{
				Vector2 P(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
				glVertex2f(P.x, P.y);
				P = Vector2(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
				glVertex2f(P.x*0.7f, P.y*0.7f);
			}
			glEnd();
			glEndList();
	}

	void grLine(const Vector2 &v0, const Vector2 &v1);
	void grSegment(const Vector2 &v0, const Vector2 &v1);
	void grSegmentC(const Vector2 &v0, const Vector2 &v1);

	void grRectL(const Vector2 &p, scalar width, scalar height);
	void grRectS(const Vector2 &p, scalar width, scalar height);
	
	void grCircleL(const Vector2 &p, scalar Radius);
	void grCircleS(const Vector2 &p, scalar Radius);
	void grCircleC(const Vector2 &p, scalar Radius);

	void grPolyC(const Vector2 &p, scalar angle, CPolygon *poly);

	void grRingL(const Vector2 &p, scalar Radius);
	void grRingS(const Vector2 &p, scalar Radius);

	void gRenderArrow(const Vector2& v0, const Vector2& v1);
	void gRenderArrowEx(const Vector2 &v0,const Vector2 &v1);

	void gDrawBBox(CBBox box);
	void grInYan(scalar Radius);
private:
	void BeforeRndr();
	void AfterRndr();
	void CheckBlend();
	void CheckTexture();
};


/**
*	Controls scissor test
*/
void gToggleScissor(bool State);
void gScissor(int x, int y, int width, int height);
void gSetColor(RGBAf color);

typedef BOOL (__stdcall *PFNWGLSWAPINTERVALFARPROC)( int );


void setVSync(int interval=1);


#endif OPENGLUTILS_H