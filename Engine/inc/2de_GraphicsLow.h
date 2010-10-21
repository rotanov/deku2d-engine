#ifndef _2DE_GRAPHICS_LOW_H_
#define _2DE_GRAPHICS_LOW_H_

#include "2de_Core.h"
#include "2de_Update.h"

#include "2de_ImageUtils.h"
#include "2de_Math.h"
#include "2de_Resource.h"

#define _2DE_DEBUG_DRAW_BOXES


// OpenGL is not included in the interface now, so I'll redefine it myself
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef int GLsizei;

const Vector2 V2_QuadBin[4] = // Four vectors representing the quad with vertices (0 0) (1 0) (1 1) (0 1)
{
	V2_ZERO,
	V2_DIR_RIGHT,
	V2_DIR_RIGHT + V2_DIR_UP,
	V2_DIR_UP,
};

const Vector2Array<4> V2_QUAD_BIN =  Vector2Array<4>(V2_QuadBin);

const Vector2 V2_QuadBinCenter[4] = // Four vectors representing the quad with vertices (-1 -1) (1 -1) (1 1) (-1 1)
{
	V2_DIR_LEFT + V2_DIR_DOWN,
	V2_DIR_RIGHT + V2_DIR_DOWN,
	V2_DIR_RIGHT + V2_DIR_UP,
	V2_DIR_LEFT + V2_DIR_UP,
};

const Vector2Array<4> V2_QUAD_BIN_CENTER = Vector2Array<4>(V2_QuadBinCenter);


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

enum EModelType
{
	MODEL_TYPE_LINES,
	MODEL_TYPE_POINTS,
	MODEL_TYPE_TRIANGLES,
};

enum EBlendingMode
{
	BLEND_MODE_ADDITIVE,
	BLEND_MODE_OPAQUE,
	BLEND_MODE_TRANSPARENT,
};

class CTransformation
{
private:
	float DepthOffset;
	Vector2 Translation;
	float Rotation;
	//Matrix2 RotationalMatrix;
	float Scaling;

public:
	CTransformation(float ADepthOffset, const Vector2 &ATranslation,
		float ARotation, float AScaling) : DepthOffset(ADepthOffset),
		Translation(ATranslation), Rotation(ARotation), Scaling(AScaling)
	{

	}

	CTransformation& operator +=(const CTransformation &rhs)
	{
		DepthOffset += rhs.DepthOffset;
		Translation += rhs.Translation;
		Rotation += rhs.Rotation;
		Scaling += rhs.Scaling;
		return *this;
	}

	CTransformation& operator -=(const CTransformation &rhs)
	{
		DepthOffset -= rhs.DepthOffset;
		Translation -= rhs.Translation;
		Rotation -= rhs.Rotation;
		Scaling -= rhs.Scaling;
		return *this;
	}

	float GetDepth() const 
	{
		return DepthOffset;
	}

	const Vector2& GetPosition() const 
	{
		return Translation;
	}

	float GetAngle() const
	{
		return Rotation;
	}

	float GetScaling() const
	{
		return Scaling;
	}

	void Clear()
	{
		DepthOffset = 0.0f;
		Translation = V2_ZERO;
		Scaling = 1.0f;
		Rotation = 0.0f;
	}

// 	Matrix2 GetRotationalMatrix();
// 	Matrix3 GetTransformationMatrix();

};

class CRenderObjectInfo
{
public:

	Vector2 Position;
	RGBAf Color;
	bool doIgnoreCamera; // if true, then object will be drawn in global cords,
	bool doMirrorHorizontal;
	bool doMirrorVertical;

	CRenderObjectInfo();
	void SetAngle(float AAngle = 0.0f);
	float GetAngle() const;
	float GetScaling() const;
	void SetScaling(float AScaling);
	void SetLayer(int Layer); // Layers should be from SOME_NEGATIVE_VALUE to SOME_POSITIVE_VALUE. Layer with greater number is drawn over layer with lower one.
	int GetLayer() const;
	float GetDepth() const;

private:
	float Angle; //	(Degrees)
	float Depth; //	[-1; 1]?
	float Scaling;
	
};

class CAbstractScene;

class CRenderable : public virtual CObject, public CRenderObjectInfo
{
public:
	CRenderable();
	virtual ~CRenderable();
	virtual void Render() = 0;
	const CBox GetBox() const;
	float Width();
	float Height();
	virtual void SetBox(const CBox &ABox);
	bool GetVisibility() const;
	virtual void SetVisibility(bool AVisible);
	void PutIntoScene(CAbstractScene *AScene);
	CAbstractScene* GetScene() const;

private:
	CBox Box; //	Axis Aligned Bounding Box for culling
	CAbstractScene *Scene;
	bool Visible;
};

class CModel // : public 
{
/*
	CTexture*
	Vector2*
	Vector2*
	RGBAf*
*/
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

	bool Load();
	void Unload();

	bool SaveToFile(const string &AFilename);

	GLuint GetTexID();

protected:

private:
	
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
class CPrmitiveVertexDataHolder	// for any non textured stuff
{
	friend class CRenderManager;
public:
	CPrmitiveVertexDataHolder();
	virtual ~CPrmitiveVertexDataHolder();
	virtual void PushVertex(const CRenderObjectInfo *Sender, const Vector2 &Vertex, const RGBAf &Color);
	virtual void PushVertex(const CRenderObjectInfo *Sender, const Vector2 &Vertex);
	virtual void RenderPrimitive(GLenum Type);
	void Clear();
	unsigned int GetVertexCount();

protected:
	static const unsigned int StartSize = 256;
	unsigned int VertexCount;
	unsigned int ReservedCount;
	RGBAf *Colors;
	Vector3 *Vertices;

	virtual void Grow();
};

class CVertexDataHolder : public CPrmitiveVertexDataHolder
{
	friend class CRenderManager;
public:
	CVertexDataHolder();
	~CVertexDataHolder();
	void PushVertex(const CRenderObjectInfo *Sender, const Vector2 &Vertex, const RGBAf &Color);
	void PushVertex(const CRenderObjectInfo *Sender, const Vector2 &Vertex);
	void PushVertex(const CRenderObjectInfo *Sender, const Vector2 &Vertex,
		const RGBAf &Color, const Vector2 &TexCoord);
	void PushVertex(const CRenderObjectInfo *Sender, const Vector2 &Vertex, const Vector2 &TexCoord);
	void RenderPrimitive(GLenum Type);

private:
	Vector2 *TexCoords;

	void Grow();
};

//////////////////////////////////////////////////////////////////////////
// CAbstractRender

class CTransformator	// lol
{
public:
	virtual void PushTransformation(const CRenderObjectInfo * ATransformation)
	{
		CTransformation TempTransformation(ATransformation->GetDepth(), ATransformation->Position,
			ATransformation->GetAngle(), ATransformation->GetScaling());
		CurrentTransformation += TempTransformation;
		TransformationStack.push_back(TempTransformation);
	}
	virtual void PopTransformation()
	{
		CurrentTransformation -= TransformationStack.back();
		TransformationStack.pop_back();
	}

	virtual void ClearTransformation()
	{
		TransformationStack.clear();
		CurrentTransformation.Clear();
	}

protected:
	CTransformation CurrentTransformation;
	vector<CTransformation> TransformationStack;
};

class CAbstractRenderer
{
public:
	virtual bool Initialize() = 0;
	virtual bool Finalize() = 0;
	virtual void PushModel(EModelType, CRenderObjectInfo *, Vector2 *) = 0;
	virtual void PushModel(EModelType, CRenderObjectInfo *, GLuint, Vector2 *) = 0;

	virtual void Render() = 0;

protected:

private:

};

class CFixedPipelineRenderer : public CAbstractRenderer
{
public:
	bool Initilize()
	{

	}

	bool Finalize()
	{

	}

	void PushModel(EModelType ModelKind, CRenderObjectInfo *RenderInfo, Vector2 *Vertices)
	{
		CPrmitiveVertexDataHolder *VertexHolder = &PrimitiveHolders[ModelKind];
// 		switch (EModelKind)
// 		{
// 		case MODEL_KIND_POINTS:
// 			break;
// 		case MODEL_KIND_LINES:
// 			break;
// 		case MODEL_KIND_TRIANGLES:
// 			break;
// 		}
	}

	void PushModel(EModelType, CRenderObjectInfo *, GLuint, Vector2 *)
	{

	}

	void Render()
	{

	}

private:
	CPrmitiveVertexDataHolder PrimitiveHolders[MODEL_TYPE_TRIANGLES + 1];
};

//////////////////////////////////////////////////////////////////////////
//CFont

#define CFONT_DEFAULT_DISTANCE	1				// FFFUUU~
#define CFONT_MAX_SYMBOLS			256

class CFont : public CResource
{
public:
	CBox Boxes[256];	// why 256? // TODO: make this private and getter that checks for FirstTimeLoaded..

	CFont();
	~CFont();

	bool Load();
	void Unload();

	bool SaveToFile(const string &AFilename);

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

	friend class CRenderManager;
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
	Vector2 Point;
	CBox	view, outer, world;
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
	Vector2 GetTranslation() const
	{
		return Vector2(static_cast<int>(w * 0.5f - Point.x), static_cast<int>(h * 0.5f - Point.y));
	}
};


class CFontManager : public CCommonManager <list <CFont*> >, public CTSingleton <CFontManager>
{
public:
	void Init();

	CFont* GetDefaultFont()
	{
		assert(DefaultFont != NULL);
		return DefaultFont;
	}

	CFont* GetFont(const string &fontname);

protected:	
	CFontManager();
	~CFontManager();
	friend class CTSingleton<CFontManager>;

private:
	CFont *DefaultFont;
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
	friend class CEngine;
private:
	CAbstractRenderer *Renderer;
	#define DEKU2D_MAX_TEXTURES 32
	CVertexDataHolder TexturedQuadVertices[DEKU2D_MAX_TEXTURES];	// Временно. В вектор запихать, что ли.
	CPrmitiveVertexDataHolder PointVertices;
	CPrmitiveVertexDataHolder LineVertices;
	CPrmitiveVertexDataHolder QuadVertices;
	CPrmitiveVertexDataHolder TriVertices;

	void SetBlendingMode();
	void BeginFrame();
	void EndFrame();
public:
	CCamera	Camera;
	~CRenderManager();
	bool DrawObjects();
	void Print(const CText *Text, const string &Characters);
	void DrawLinedBox(const CRenderObjectInfo* RenderInfo, const CBox &Box);
	void DrawSolidBox(const CRenderObjectInfo* RenderInfo, const CBox &Box);
	// Box is always box, but TC may be used to mirros sprite or smthng, so it is not CBox
	void DrawTexturedBox(const CRenderObjectInfo* RenderInfo, const CBox &Box, CTexture *Texture, const Vector2Array<4> &TexCoords);
	void DrawPoint(const CRenderObjectInfo *RenderInfo, const Vector2 &Point);
	void DrawLine(const CRenderObjectInfo *RenderInfo, const Vector2 &v0, const Vector2 &v1);
	void DrawTriangles(const CRenderObjectInfo *RenderInfo, const Vector2 *Vertices, unsigned int Count);
};

#if defined(_WIN32)
	#define SWAP_INTERVAL_PROC PFNWGLSWAPINTERVALFARPROC 
#elif defined(__linux)
	#define SWAP_INTERVAL_PROC PFNGLXSWAPINTERVALSGIPROC
#endif
typedef int (APIENTRY *SWAP_INTERVAL_PROC)(int);
void setVSync(int interval = 1);

//////////////////////////////////////////////////////////////////////////
//CGLWindow

class CGLWindow : public CTSingleton<CGLWindow>
{
public:
	bool gCreateWindow(bool AFullscreen, int AWidth, int AHeight, byte ABPP, const string &ACaption);
	bool gCreateWindow();
	void SetSize();
	bool glInitSystem();
	void glSuicide();
	void glResize(GLsizei Width, GLsizei Height);
	void glInit(GLsizei Width, GLsizei Height);
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	void SetFullscreen(bool AFullscreen);
	void SetBPP(byte ABPP);
	void SetWidth(unsigned int AWidth);
	void SetHeight(unsigned int AHeight);
	string GetCaption() const;
	void SetCaption(const string &ACaption);
	void SetBackgroundColor(const RGBAf &AColor);

protected:
	CGLWindow();
	friend class CTSingleton<CGLWindow>;

private:
	byte BPP;
	string Caption;
	unsigned int Height;
	bool isCreated;
	bool isFullscreen;
	unsigned int Width;	
};
//////////////////////////////////////////////////////////////////////////
//CGrRect - not nice enough, i think.

class CBasePrimitive : public CRenderObjectInfo
{
public:
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
		CRenderManager::Instance()->DrawLine(this, Segment.v0, Segment.v1);
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
		CGrLine::Render();
		CRenderManager::Instance()->DrawPoint(this, Segment.v0);
		CRenderManager::Instance()->DrawPoint(this, Segment.v1);
	}
};

class CGrRect : public CBasePrimitive
{
public:
	CRectangle Rectangle;

	CGrRect();
	CGrRect(const Vector2 p0, const Vector2 p1) : Rectangle(p0, p1)
	{		
	}
	~CGrRect()
	{

	}
	void Render()
	{
		CRenderManager::Instance()->DrawSolidBox(this, CBox(Rectangle.v0, Rectangle.v2));
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
	//const CBox GetBox() const;
	CFont* GetFont() const;
	string& GetText();
	const string& GetText() const;
	void SetFont(CFont *AFont);
	void SetText(const string &AText);
	CText& operator =(const string &AText);
	unsigned char operator [] (unsigned int index) const;
	float StringCoordToCursorPos(int x, int y) const;	// Тот кто это писал - объясни, зачем нам передавать "y"?
	unsigned int Length() const;

private:
	string Characters;
	CResourceRefCounter<CFont> Font;
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
// CMouseCursor
// TODO: <strike>сделать курсор отдельным классом</strike>, рисовать на максимальном слое, возможность загрузки текстурных курсоров, etc.
class CMouseCursor : public CRenderable
{
public:
	CBox Box;
	CMouseCursor()
	{		
		Box = CBox(Vector2(-2.0f, -2.0f), Vector2(1.0f, 1.0f));
		SetLayer(512);
		Color = COLOR_GREEN;
	}
	void Render()
	{
		CRenderManager::Instance()->DrawLinedBox(this, Box);
	}
};

#endif // _2DE_GRAPHICS_LOW_H_

