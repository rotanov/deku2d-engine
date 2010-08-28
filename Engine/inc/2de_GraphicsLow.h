#ifndef _2DE_GRAPHICS_LOW_H_
#define _2DE_GRAPHICS_LOW_H_

#include "2de_Core.h"
#include "2de_Update.h"

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
*	Виртуально унаследовать от этой гадости - это первый вариант. // Интересно, почему я думал унаследовать виртуально?
*	Второй вариант - аггрегировать указатель на неё в классы.
*/

class CRenderableUnitInfo
{
public:
	Vector2 Position;
	RGBAf Color;	
	bool doIgnoreCamera;	// if true, then object will be drawn in global cords,

	CRenderableUnitInfo();
	void SetAngle(float AAngle = 0.0f);
	float GetAngle() const;
	float GetScaling() const;
	void SetScaling(float AScaling);
	void SetLayer(unsigned int Layer);	// Layers should be from 0 to SOME_POSITIVE_VALUE. Layer with greater number is drawn over layer with lower one.
	unsigned int GetLayer() const;
	float GetDepth() const;

private:
	float				Angle;		//	(Degrees)
	float				Depth;		//	[-1; 1]?
	float				Scaling;	
};

class CAbstractScene;

class CRenderable : public virtual CObject, public CRenderableUnitInfo
{
public:
	CRenderable();
	virtual ~CRenderable();
	virtual void Render() = 0;
	const CBox GetBox() const;
	virtual void SetBox(const CBox &ABox);
	bool GetVisibility() const;
	virtual void SetVisibility(bool AVisible);
	void PutIntoScene(CAbstractScene *AScene);
	CAbstractScene* GetScene() const;

private:
	CBox				Box;		//	Axis Aligned Bounding Box for culling
	CAbstractScene		*Scene;
	bool				Visible;
};

//////////////////////////////////////////////////////////////////////////
//CGLImageData

class CGLImageData : public CImageData
{
public:
	CGLImageData();
	virtual ~CGLImageData();
	bool			LoadTexture(const string &Filename);
	bool			LoadTexture(size_t AWidth, size_t AHeight, const byte* Address);	// From memory
	virtual GLuint	GetTexID();

protected:
	GLuint			TexID;

private:
	bool			MakeTexture();
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
// CIHaveNotDecidedHowToNameThisClassYet.

/*

1. A bunch of rectangles. Can use CAABB for representation;
	A bunch => we need container to hold them.
2. For each frame rectangle we have some geometery data, describing it contents
	Geometry data is one or more than one of these primitives: sircle, rectangle, convex polygon, concave polygon, something else
3. That's all? May be. But we also want to work with this structure.
4. No that's not all. If we'll be indexing all frames together than each frame should know, to which texture it belongs. So texture name;
5. Also, it'll be better if there will be some tag system, to say, e.g. "these are frames for some animation"

>We want some global indexing of these frames.
>Then want access by index, to get frame info
>Also, to add new frames
>And to check for data integrity. I mean if some places referring to some frames and we are changing these frames (e.g. deleting them)
	we want to note it. (mb this is optional or not necessary)
>And i thinking about to include those frames stuff directly into rendering, may be as some kind of bridge

*/

class CFrameInfo
{
	CBox Reactangle;
	vector<CGeometry> Primitives;
	vector<string> Tags;
};

class CFrameSequence
{
public:
	vector<CFrameInfo> Rectangles;

};

//////////////////////////////////////////////////////////////////////////
//CArrayHolder
template <unsigned int StartSize = 65536>
class CPrmitiveVertexDataHolder	// for any other non textured stuff
{
	friend class CRenderManager;
public:
	CPrmitiveVertexDataHolder();
	virtual ~CPrmitiveVertexDataHolder();
	virtual void PushVertex(const CRenderableUnitInfo *Sender, const Vector2 &Vertex, const RGBAf &Color);
	virtual void PushVertex(const CRenderableUnitInfo *Sender, const Vector2 &Vertex);
	virtual void RenderPrimitive(GLenum Type);
	void Clear()
	{
		VertexCount = 0;
	}

protected:
	unsigned int VertexCount;
	unsigned int ReservedCount;
	RGBAf *Colors;
	Vector3 *Vertices;
};

template <unsigned int StartSize = 65536>
class CVertexDataHolder : public CPrmitiveVertexDataHolder<StartSize>
{
	friend class CRenderManager;
public:
	CVertexDataHolder();
	~CVertexDataHolder();
	void PushVertex(const CRenderableUnitInfo *Sender, const Vector2 &Vertex, const RGBAf &Color);
	void PushVertex(const CRenderableUnitInfo *Sender, const Vector2 &Vertex,
		const RGBAf &Color, const Vector2 &TexCoord);
	void RenderPrimitive(GLenum Type);

private:
	Vector2 *TexCoords;
};

//////////////////////////////////////////////////////////////////////////
//CFont

#define CFONT_DEFAULT_DISTANCE	1				// FFFUUU~
#define CFONT_MAX_STRING_LENGTH		256
#define CFONT_MAX_SYMBOLS			256

class CFont : public CResource
{
	friend class CRenderManager;
public:
	CBox bbox[256];	// why 256?

	CFont();
	~CFont();
	bool LoadFromFile();
	bool SaveToFile();
	bool LoadFromMemory(const byte* Address);
	float GetStringWidth(const string &text);
	float GetStringWidthEx(int t1, int t2, const string &text);
	float GetStringHeight(const string &text);
	float GetStringHeightEx(int t1, int t2, const string &text);
	CTexture* GetTexture();
	void SetTexture(const string &TextureName);
	CBox GetSymbolsBBOX();
	float GetDistance() const;
	float SymbolWidth(unsigned int Index) const;
	Vector2Array<4> GetTexCoords(unsigned int Charachter);
	//const Vector2Array<4>& GetTexCoords(unsigned int Charachter) 
	// <-- warning: reference to local variable ‘result’ returned;
	// This function is not for user, so there should be some kind of guard
	// To let using of this function only for what needs it

private:
	float Distance;
	float Width[CFONT_MAX_SYMBOLS];
	float Height[CFONT_MAX_SYMBOLS];
	CTexture* Texture;
	GLuint base;
};

//////////////////////////////////////////////////////////////////////////
// CRenderProxy

class CRenderProxy : public CRenderable
{
private:
	CRenderable *RenderSource;

public:
	CRenderProxy(CRenderable *ARenderSource);
	void Render();
};

//////////////////////////////////////////////////////////////////////////
//Camera

class CCamera : public CObject
{
public:
	Vector2 Point;					//
	CBox	view, outer, world;		//
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
	~CFontManager();
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
class CGrRect;
class CGrPoint;
class CGrLine;

class CRenderManager : public CCommonManager <list <CRenderable*> >, public CTSingleton <CRenderManager>
{
protected:
	CRenderManager();
	friend class CTSingleton <CRenderManager>;
private:
	CVertexDataHolder<> FontVertices;	// We want them for each font, don't we? @todo: implement this possibiliy
	CPrmitiveVertexDataHolder<512>	PointVertices;
	CPrmitiveVertexDataHolder<>	LineVertices;
	CPrmitiveVertexDataHolder<>	QuadVertices;
public:
	CCamera	Camera;
	~CRenderManager();
	bool DrawObjects();
	void Print(const CText *Text);
	void DrawBox(const CGrRect *Rectangle);
	void DrawPoint(const CGrPoint *Point);
	void DrawLine(const CGrLine *Line);
};

//////////////////////////////////////////////////////////////////////////
// OpenGl interface// Херня какая-то передумать и переделать нахуй...

void gSetBlendingMode();
void gBeginFrame();
void gEndFrame();
void gToggleScissor(bool State);
void gScissor(int x, int y, int width, int height);

#if defined(_WIN32)
	#define SWAP_INTERVAL_PROC PFNWGLSWAPINTERVALFARPROC 
#elif defined(__linux)
	#define SWAP_INTERVAL_PROC PFNGLXSWAPINTERVALSGIPROC
#endif
typedef int (APIENTRY *SWAP_INTERVAL_PROC)(int);
void setVSync(int interval=1);

//////////////////////////////////////////////////////////////////////////
//CGLWindow

class CGLWindow : public CTSingleton<CGLWindow>
{
public:
	byte BPP;
	bool Fullscreen;

	bool gCreateWindow(bool AFullscreen, int AWidth, int AHeight, byte ABPP, const string &ACaption);
	bool gCreateWindow();
	void SetSize();
	bool glInitSystem();
	void glSuicide();
	void glResize(GLsizei Width, GLsizei Height);
	void glInit(GLsizei Width, GLsizei Height);
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	void SetWidth(unsigned int AWidth);
	void SetHeight(unsigned int AHeight);
	string GetCaption() const;
	void SetCaption(const string &ACaption);
	void SetBackgroundColor(const RGBAf &AColor);

protected:
	CGLWindow();
	friend class CTSingleton<CGLWindow>;
private:
	unsigned int Width;
	unsigned int Height;
	string Caption;
	bool isCreated;
};
//////////////////////////////////////////////////////////////////////////
//CGrRect - not nice enough, i think.

class CBasePrimitive : public CRenderableUnitInfo
{
public:
};

class CGrPoint : public CBasePrimitive
{
public:
	void Render()
	{
		CRenderManager::Instance()->DrawPoint(this);
	}
};


class CGrLine : public CBasePrimitive
{
public:
	CSegment Segment;
	CGrLine(const Vector2 &v0, const Vector2 &v1) : Segment(v0, v1)
	{

	}
	void Render()
	{
		CRenderManager::Instance()->DrawLine(this);
	}
};

class CGrSegMent : public CGrLine
{
public:
	CGrSegMent(const Vector2 &v0, const Vector2 &v1) : CGrLine(v0, v1)
	{

	}
	void Render()
	{
		CGrPoint p0, p1;
		p0.Position = Segment.v0;
		p1.Position = Segment.v1;
		p0.Render();
		p1.Render();
		CGrLine::Render();
	}
};

class CGrRect : public CBasePrimitive
{
public:
	CRectangle Rectangle;
	bool isLineDrawn;

	CGrRect();
	CGrRect(const Vector2 p0, const Vector2 p1) : Rectangle(p0, p1), isLineDrawn(false)
	{		
	}
	~CGrRect()
	{

	}
	void Render()
	{
// 		if (isLineDrawn)
// 		{
// 			CGrSegMent s0(Rectangle.v0, Rectangle.v1);
// 			CGrSegMent s1(Rectangle.v1, Rectangle.v2);
// 			CGrSegMent s2(Rectangle.v2, Rectangle.v3);
// 			CGrSegMent s3(Rectangle.v3, Rectangle.v0);
// 			s0.Render();
// 			s1.Render();
// 			s2.Render();
// 			s3.Render();
// 			return;
// 		}
		CRenderManager::Instance()->DrawBox(this);
	}
private:	
};
//////////////////////////////////////////////////////////////////////////
//CText
class CText : public CRenderable
{
public:
	CText();
	~CText();
	CText(const string &AText);
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
	unsigned char operator [] (unsigned int index) const
	{
		return Text[index];
	}
	float StringCoordToCursorPos(int x, int y) const;

private:
	string Text;
	CFont *Font;
};

//////////////////////////////////////////////////////////////////////////
//AbstractScene

class CAbstractScene : public CObject
{
	friend class CSceneManager;
public:
	virtual void AddRenderable(CRenderable *AObject) = 0;
	virtual void AddUpdatable(CUpdatable *AObject) = 0;
	virtual void RemoveRenderable(CRenderable *AObject) = 0;
	virtual void RemoveUpdatable(CUpdatable *AObject)= 0;

protected:
	virtual void Render() = 0;
	virtual void Update(float dt) = 0;

	CAbstractScene();
	~CAbstractScene(){}

private:
	CAbstractScene(const CAbstractScene &AScene);
	CAbstractScene& operator=(const CAbstractScene &AScene);
};

//////////////////////////////////////////////////////////////////////////
//Scene

class CScene : public CAbstractScene
{
	friend class CSceneManager;
public:
	virtual void AddRenderable(CRenderable *AObject);
	virtual void AddUpdatable(CUpdatable *AObject);
	virtual void RemoveRenderable(CRenderable *AObject);
	virtual void RemoveUpdatable(CUpdatable *AObject);

protected:
	vector<CRenderable*> RenderableObjects;
	vector<CUpdatable*> UpdatableObjects;
	virtual void Render();
	virtual void Update(float dt);
	CScene(){}
	~CScene();
};

//////////////////////////////////////////////////////////////////////////
//Global Scene

class CGlobalScene : public CScene
{
public:
	void AddRenderable(CRenderable *AObject);
	void AddUpdatable(CUpdatable *AObject);
};

//////////////////////////////////////////////////////////////////////////
//Scene Manager

class CSceneManager : CCommonManager <list<CScene*> >, public CTSingleton<CSceneManager>
{
	friend class CTSingleton<CSceneManager>;
public:	
	CAbstractScene* CreateScene();
	void DestroyScene(CAbstractScene *AScene);
	CAbstractScene* GetCurrentScene();
	void SetCurrentScene(CAbstractScene *AScene)
	{
		assert(AScene != NULL);
		CurrentScene = AScene;
	}
	bool InScope(CAbstractScene * AScene) const;
	void Render();
	void Update(float dt);
private:
	CAbstractScene *CurrentScene;
	CGlobalScene GlobalScene;
	vector<CAbstractScene*> Scenes;
protected:
	~CSceneManager();
	CSceneManager();
};

//////////////////////////////////////////////////////////////////////////
// Implementation section
//////////////////////////////////////////////////////////////////////////
//CPrimitiveVertexDataHolder

template <unsigned int StartSize>
CPrmitiveVertexDataHolder<StartSize>::CPrmitiveVertexDataHolder() : VertexCount(0), ReservedCount(StartSize), Colors(NULL), Vertices(NULL)
{
	Colors = new RGBAf [StartSize];
	Vertices = new Vector3 [StartSize];
}

template <unsigned int StartSize>
CPrmitiveVertexDataHolder<StartSize>::~CPrmitiveVertexDataHolder()
{
	delete [] Colors;
	delete [] Vertices;
}

template <unsigned int StartSize>
void CPrmitiveVertexDataHolder<StartSize>::PushVertex( const CRenderableUnitInfo *Sender, const Vector2 &Vertex, const RGBAf &Color )
{
	assert(Sender != NULL);
	// if VertexCount == ReservedCount then grow
	Vector2 TempVector = Vertex + Sender->Position;
	Vertices[VertexCount] = Vector3(TempVector.x, TempVector.y, Sender->GetDepth()); // also apply rotation here
	Colors[VertexCount] = Color;
	VertexCount++;
}

template <unsigned int StartSize>
void CPrmitiveVertexDataHolder<StartSize>::PushVertex( const CRenderableUnitInfo *Sender, const Vector2 &Vertex )
{
	PushVertex(Sender, Vertex, Sender->Color);
}

template <unsigned int StartSize>
void CPrmitiveVertexDataHolder<StartSize>::RenderPrimitive( GLenum Type )
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, Vertices);
	glColorPointer(4, GL_FLOAT, 0, Colors);
	glDrawArrays(Type, 0, VertexCount);
	CPrmitiveVertexDataHolder<StartSize>::VertexCount = 0; // Clear here or not?
}

//////////////////////////////////////////////////////////////////////////
//CVertexDataHolder
template <unsigned int StartSize>
CVertexDataHolder<StartSize>::CVertexDataHolder()
{
	TexCoords = new Vector2 [StartSize];
}

template <unsigned int StartSize>
CVertexDataHolder<StartSize>::~CVertexDataHolder()
{
	delete [] TexCoords;
}

template <unsigned int StartSize>
void CVertexDataHolder<StartSize>::PushVertex( const CRenderableUnitInfo *Sender, const Vector2 &Vertex, const RGBAf &Color )
{
	assert(false);
}

template <unsigned int StartSize>
void CVertexDataHolder<StartSize>::PushVertex( const CRenderableUnitInfo *Sender, const Vector2 &Vertex, const RGBAf &Color, const Vector2 &TexCoord )
{
	CPrmitiveVertexDataHolder<StartSize>::PushVertex(Sender, Vertex, Color);
	// if VertexCount == ReservedCount then grow here also
	TexCoords[CPrmitiveVertexDataHolder<StartSize>::VertexCount - 1] = TexCoord;
}

template <unsigned int StartSize>
void CVertexDataHolder<StartSize>::RenderPrimitive( GLenum Type )
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, CPrmitiveVertexDataHolder<StartSize>::Vertices);
	glColorPointer(4, GL_FLOAT, 0, CPrmitiveVertexDataHolder<StartSize>::Colors);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoords);
	glDrawArrays(Type, 0, CPrmitiveVertexDataHolder<StartSize>::VertexCount);
	CPrmitiveVertexDataHolder<StartSize>::VertexCount = 0;
}

#endif // _2DE_GRAPHICS_LOW_H_
