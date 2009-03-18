#ifndef OPENGLUTILS_H
#define OPENGLUTILS_H

#include <stdarg.h>
#include <SDL.h>

#include "CoreUtils.h"

#ifdef USE_SDL_OPENGL
	#include "SDL_opengl.h"
#else
	#include <gl\gl.h>
	#include <gl\glu.h>
#endif

#include "ImageUtils.h"
#include "MathUtils.h"
#define LOWBYTE(bt) (bt&0xff)
#define _GetRValue(rgb)      (LOWBYTE(rgb))
#define _GetGValue(rgb)      (LOWBYTE(((WORD)(rgb)) >> 8))
#define _GetBValue(rgb)      (LOWBYTE((rgb)>>16))

// #define _DEBUG_DISABLE_PARTICLES_DRAW
// #define _DEBUG_DISABLE_PARTICLES_UPDATE

class CPrimitiveRender;
class CParticleSystem;

class CRenderObject : public virtual CObject
{
public:
	Vector2				p;
	float				depth;
	bool				visible;
	RGBAf				color;
	CRenderObject() : p(V2Zero), depth(0.0f), visible(true), color(1.0f, 1.0f, 1.0f, 1.0f)
	{
		name += "CRenderObject ";
		type |= T_RENDERABLE;
	};
	void SetColor(byte _r, byte _g, byte _b, byte _a);
	virtual bool Render() = 0;
	virtual bool RenderByParticleSystem(const Vector2 &p, const CParticleSystem &ps)
	{
		return true;
	};
	virtual ~CRenderObject(){};
};

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*			CPrimitiveRender & stuff									*/
/************************************************************************/

#define G_PRIM_BLEND_OPT
#define G_POLY_TEX_CELL_SIZE 20
#define G_POLY_OUTLINE_ENABLE
#define G_POLY_TEXTURE_ENABLE

#define PRM_RNDR_OPT_BLEND_ONE		0x01
#define PRM_RNDR_OPT_BLEND_OTHER	0x02

class CPrimitiveRender
{
public:
	int BlendingOption, CheckerCellSize;
	bool OutlineEnabled, TextureEnabled, DashedLines, doUseCurrentCoordSystem, LineStippleEnabled;
	static int glListCircleL;
	static int glListCircleS;
	static int glListRingS;
	static int glListHalfCircle;
	scalar Angle, lwidth, depth, psize;
	int dash;
	RGBAf *plClr, *psClr, *ppClr;
	RGBAf lClr, sClr, pClr; 
	CPrimitiveRender()
	{
		lClr = sClr = pClr = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
		plClr = &lClr;
		psClr = &sClr;
		ppClr = &pClr;
		depth = 0.0f;
		lwidth = 1.0f;
		psize = 8.0f;
		LineStippleEnabled = doUseCurrentCoordSystem = false;			
	}

	void Init();

	/**
	*	����� ������� ��� ����������� ����������.
	*	������� L - �������� �������� �������
	*	S - solid
	*	C - complete
	*	��������� - �� ������� ��� ������� ���������.
	*	������ - �������� + ����� + ������� + ��� ���-��
	*/

	void grLine(const Vector2 &v0, const Vector2 &v1);
	void grSegment(const Vector2 &v0, const Vector2 &v1);
	void grSegmentC(const Vector2 &v0, const Vector2 &v1);

	void grRectL(const Vector2 &v0, const Vector2 &v1);
	void grRectS(const Vector2 &v0, const Vector2 &v1);
	void grRectC(const Vector2 &v0, const Vector2 &v1);

	void grCircleL(const Vector2 &p, scalar Radius);
	void grCircleS(const Vector2 &p, scalar Radius);
	void grCircleC(const Vector2 &p, scalar Radius);

	void grPolyC(const Vector2 &p, scalar angle, CPolygon *poly);

	void grRingS(const Vector2 &p, scalar Radius);
	void grRingC(const Vector2 &p, scalar Radius);

	void grArrowL(const Vector2& v0, const Vector2& v1);
	void grArrowC(const Vector2 &v0,const Vector2 &v1);

	void gDrawBBox(CBBox box);
	void grInYan(const Vector2 &p, scalar Radius);
private:
	void BeforeRndr();
	void AfterRndr();
	void CheckBlend();
	void CheckTexture();
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
	virtual GLuint GetTexID();

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
	GLuint GetTexID();
protected:
	void Unload()
	{
		glDeleteTextures(1, &TexID);
	}
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

struct SAnimationInfo				// ��������� ������������ ��������� ��������
{
	bool	isAnimated;				// ���������� �� ������
	bool	isLoop;					// ��������� �� ��������
	float	m_fFrameDelay;			// �������� ����� ������� � ��
	float	m_fwidth, m_fheight;	// �������� ������ � ������ ������� ��� ����������� �� ������
			
	int	m_nNumFrameColumns,			// ���-�� ������� � ��������
		m_nNumFrameRows,			// ���-�� ����� � ��������
		m_nTotalFrames,				// ���-�� ������ � ��������
		m_nFrameWidth,				// ������ ����� � ��������
		m_nFrameHeight,				// ������ ����� � ��������
		m_nOffsetX,					// ���������� �� ������ ���� �� ������� ����� � �������� �� �
		m_nOffsetY;					// ���������� �� ������ ���� �� ������� ����� � �������� �� �
	int AnimationIndex;				// ������ ��������
};

class CSprite : public CRenderObject
{
public:
	int			m_nTextureWidth,		// ������ �������� � ��������
				m_nTextureHeight;		// ����, �� �����
	RGBAf		color;					// ����
	GLuint		m_textureID;			// ������������ ����� �������. ��������

	DWORD		ellapsedtime, lasttime;	// ���� ����� ������ ����� ����� �������
	int			m_nFrameNumber,			// ����� �������� �����
				m_nCurrentRow,			// ����� ������� ������
				m_nCurrentColumn;		// ����� ������� ������

	bool		m_bFirstRendering;		// ������ �� ��� ���������� ��������� �������
	byte		numAnimations;			// ���-�� �������� ��� ������ �������

	SAnimationInfo *animations;		// ��������� ��������. ����������� �� �����
	SAnimationInfo *anim;			// ��������� �� ������� ��������
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

#define CFONT_DEFAULT_DISTANCE	1
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
};

#define CFONT_DEPTH_HIGH		1.0f
#define CFONT_DEPTH_LOW			0.0f
#define CFONT_MAX_STRING_LENGTH	256

class CFont : public CObject
{
public:
	CFont();
	~CFont();
	
	RGBAf				tClr;					//	����.
	byte				dist;					//	���������� ����� ���������		
	Vector2				p;						//	��������� ������, ��� ������������, ��������� �� ������� ��������� �� ���
	Vector2				wh;						//	������ � ������� � ������� ����-��. ��� ��� ������. x - w, y - h
	int					offset;					//	��������, � �������� ���� �������� � �����, ���� �� ������� �� ���������
	int					s1, s2;					//	������ ������� � ���������� �������� ����������� ������
	bool				isRect;					//	���� �� �������� ����� � �������������
	bool				isSelected;				//	������� �� ����� ������
	bool				doUseGlobalCoordSystem;	//	������������ �� ��� ������ ���������� ������� ��������	
	CPrimitiveRender	Prndr;					//	��������� ����� � ������ ����� �������� � ������.

	bool		LoadFromFile(char* filename);
	bool		SaveToFile(char* filename);

	void		Print(const char *text, ...);

	void		SetDepth(float _depth);
	void		PointTo(const Vector2 *_p);
	void		PointBack();
	void		SetAlign(const byte _Halign, const byte _Valign);

	int			GetStringWidth(const char *text);
	int			GetStringWidthEx(int t1, int t2, const char *text);
	int			GetStringHeight(const char *text);
	int			GetStringHeightEx(int t1, int t2, const char *text);

private:
	float				x, y;					//	����������� ���������� ��� ��������� � _Print
	float				depth;					//	������� �� Z
	(const Vector2)		*pp;					//	��������� �� ������ � ������������ ������
	byte				align;					//	����� ������������

	CRecti		bbox[256];		// �������� ���� ������� ��� ������� �������
	byte		width[256];		// ������ ������� �������
	byte		height[256];	// ������ ������� �������
	char		*FontImageName;	// ��� ����� ��������
	GLuint		font;			// Font texture ID
	GLuint		base;			// Base List of 256 glLists for font

	void		_Print(const char *text);
	byte		GetHalign();
	byte		GetValign();
};

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


//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*				CparticleSystem & stuff									*/
/************************************************************************/

int Random_Int(int min, int max);
float Random_Float(float min, float max);

struct CParticle
{
	Vector2			p;			//	������� �������
	Vector2			v;			//	�������� 
	RGBAf			c;			//	������� ����
	RGBAf			dc;			//	���������� ����� �� ������ ����
	float			size;		//	������� ������
	float			dsize;		//	���������� �������
	float			life;		//	����� ����� �������; -1 ������ �������������
	float			age;		//	������� �������, �� ���� ������� ��� ��� ���������������
	float			Period;		//	��� �����, �� ���. � �� �����
};

struct CPsysteminfo
{
	Vector2			p;						//	������� ������� ������

	RGBAf			sc;						//	��������� ���� ������ �������
	RGBAf			ec;						//	�������� ���� ������ �������
	RGBAf			vc;						//	������ ����������� ��� ������ �����

	int				MaxParticles;			//	����������� �������� ����� ������
	int				ParticlesActive;		//	������� ������ ������� ������
	int				lifemin;				//	����������� ����� ����� �������
	int				lifemax;				//	������������ ����� ����� �������

	float			startsize;				//	��������� ������ ������ �������
	float			endsize;				//	�������� ������
	float			sizevar;				//	������ ����������� ��� ������ ������� �������

	float			plife;					//	���������������
	int				plifevar;				//	���������������

	float			life;					//	����� ����� �������; -1 ������ ������������
	int				age;					//	������� ������� �������

	int				emission;				//	������, ������������ ������� ������ ������� �� ���
	int				notcreated;				//	���������� ������, ������� �� �� ������ ��������� � ���� ����� �� �����-�� ��������
	Vector2			*geom;					//	������ �����, ��� ��������� ������
	int				GeomNumPoints;			//	����� ���� �����
	bool			isSnow;					//	������� ��� �����. TODO! ������ �����
};

typedef void (*FCreateFunc)(CParticle *);
typedef void (*FUpdateFunc)(CParticle *, float);

/**
*	TODO:
*		1. �������� ������������ �������, ������� ��������
*		2. �������� ��������� ���������� ������� ��� �������
*		3. �������� ��� ���-������
*/

class CParticleSystem :  public CResource, public CRenderObject, public CUpdateObject
{
public:
	CPsysteminfo			info;
	CRenderObject			*UserRenderSample;
	CParticle				*particles;
	GLuint					TexID;
	bool					user_create;
	bool					user_update;
	bool					user_render;
	void					(*procUserCreate)(CParticle *);
	void					(*procUserUpdate)(CParticle *, float);

							CParticleSystem();
							~CParticleSystem();
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
// OpenGl interface// ����� �����-�� ���������� � ���������� �����...
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

/**
*	Controls scissor test
*/
typedef BOOL (__stdcall *PFNWGLSWAPINTERVALFARPROC)(int);

void gToggleScissor(bool State);
void gScissor(int x, int y, int width, int height);
void gSetColor(RGBAf color);

void setVSync(int interval=1);


#endif OPENGLUTILS_H