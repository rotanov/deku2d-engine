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

//////////////////////////////////////////////////////////////////////////
// Various constants

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

class CAbstractScene;

/**
*	CGLImageData - like CImageData, but can load itself as a texture;
*	It's a little strange, i think, but it's working and has not caused any trouble
*	so I'll leave that hierarchy the way it is now 'til the problems occure.
*/
class CGLImageData : public CImageData
{
public:
	CGLImageData();
	virtual ~CGLImageData();
	bool LoadTexture(const string &Filename);
	bool LoadTexture(size_t AWidth, size_t AHeight, const byte* Address);	// From memory
	virtual GLuint GetTexID();

protected:
	GLuint TexID;

private:
	bool MakeTexture();
};

/**
*	CTexture — is as CGLImageData, but also has CResource functionality
*	can Load() and Unload() and even SaveToFile(). Wait, it still can't
*	save to file, but function is defined @todo: implement saving of textures to
*	file. And don't even try to think when we'll need such possibility
*/
class CTexture : public CGLImageData, public CResource
{
public:
	CTexture();
	virtual ~CTexture();
	bool Load();
	void Unload();

	/**
	*	As I think for now this one should save Texture to file.
	*	I mean we can render something to Texture, and then save it
	*	as image on HDD; The trivial example is screenshoot.
	*	Less trivial is some intermediate textures for gfx;
	*/
	bool SaveToFile(const string &AFilename);
	GLuint GetTexID();
};

/**
*	CTextureManager — actually a useless class used as a container for textures;
*	in order to access texture; So @todo: get rid of CTextureManager
*/
class CTextureManager : public CCommonManager <list <CTexture*> >, public CTSingleton <CTextureManager> 
{
protected:
	CTextureManager();
	friend class CTSingleton <CTextureManager>;
};

/**
*	CTransformation - class describes placement in space and some other properties
*	which affects to object position, orientation and size;
*/
class CTransformation	// "CPlacement"?
{
private:
	float DepthOffset;
	Vector2 Translation;
	float Rotation;
	//Matrix2 RotationalMatrix;
	float Scaling;

public:
	CTransformation(float ADepthOffset = 0.0f, const Vector2 &ATranslation = V2_ZERO,
		float ARotation = 0.0f, float AScaling = 1.0f);
	/**
	*	Don't be confused here: Look to implementation - it applies one transformation
	*	on other to represent common transformation, not just add members
	*/
	CTransformation& operator +=(const CTransformation &rhs);
	CTransformation& operator -=(const CTransformation &rhs);
	float GetDepth() const;
	const Vector2& GetPosition() const;
	float GetAngle() const;
	float GetScaling() const;
	void Clear();
// 	Matrix2 GetRotationalMatrix();
// 	Matrix3 GetTransformationMatrix();
};

/**
*	CModel — represents object geometry, object type and if has - 
*	texture and texture coordinates. 
*	@todo: should be resource;
*/
enum EModelType
{
	MODEL_TYPE_NOT_A_MODEL = -1,
	MODEL_TYPE_LINES = 0,
	MODEL_TYPE_POINTS = 1,
	MODEL_TYPE_TRIANGLES = 2,
//	MODEL_TYPE_QUADS = 3,	Keep commented 'cause may be there will be some possibility for optimisation
};

class CModel // : public // is Abstract as fuck
{
public:
	virtual ~CModel();
	virtual void SetTexture(CTexture *ATexture) = 0;
	virtual void SetModelType(EModelType AModelType) = 0;
	virtual CTexture* GetTexture() = 0;
	virtual const Vector2* GetVertices() const = 0;
	virtual const Vector2* GetTexCoords() const = 0;
	//const RGBAf* GetColors() const = 0;
	virtual EModelType GetModelType() const = 0;
	virtual int GetVertexNumber() const = 0;
};

/**
*	Configuration of object, that represents object transformation
*	and some render options.
*/
enum EBlendingMode
{
	BLEND_MODE_ADDITIVE,
	BLEND_MODE_OPAQUE,
	BLEND_MODE_TRANSPARENT,
};

class CRenderConfig
{
public:
	Vector2 Position;
	RGBAf Color;
	bool doIgnoreCamera;		// if true, then all previous transformations are ignored
	bool doMirrorHorizontal;
	bool doMirrorVertical;

	CRenderConfig();
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

/**
*	CRenderable — is no more. @todo: get rid of it and replace to CRenderableComponent;
*	Was used in such way, that we inherit from CRenderable and then implement it's 
*	void Render() so object knows how it renders itself and CRenderManager 
*	will automatically invoke Render() for all CRenderable objects;
*/
class CRenderable : public virtual CObject, public CRenderConfig
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

/**
*	CRenderableComponent - AOP way replacement for CRenderable.
*/
class CRenderableComponent : public CGameObject
{
public:
	CRenderConfig Configuration;
	CModel *Model;

	CRenderableComponent(CModel *AModel = NULL) : Model(AModel)
	{

	}
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

/**
*	CPrmitiveVertexDataHolder was useful class to incorporate Colors and Vertices
*	for pushing and drawing them. Now modern version of that class included in
*	declaration of CFFPRender and that version is for @todo: get rid of it.
*/
class CPrmitiveVertexDataHolder	// for any non textured stuff
{
	friend class CRenderManager;
public:
	CPrmitiveVertexDataHolder();
	virtual ~CPrmitiveVertexDataHolder();
	virtual void PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex, const RGBAf &Color);
	virtual void PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex);
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

/**
*	CVertexDataHolder - same as CPrmitiveVertexDataHolde, but for textured primitives
*/
class CVertexDataHolder : public CPrmitiveVertexDataHolder
{
	friend class CRenderManager;
public:
	CVertexDataHolder();
	~CVertexDataHolder();
	void PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex, const RGBAf &Color);
	void PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex);
	void PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex,
		const RGBAf &Color, const Vector2 &TexCoord);
	void PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex, const Vector2 &TexCoord);
	void RenderPrimitive(GLenum Type);

private:
	Vector2 *TexCoords;

	void Grow();
};

/**
*	CTransformator holds current transformation and stack of transformations.
*	Should be updated before use, lol.
*	Now updated in recursive traverse of CGameObject tree
*	whenever we need to apply current transformation to dome vertices
*	we use GetCurrentTransfomation(), if we know, that we are in CGameObject tree traverse
*	and it's information is actual and true.
*/
class CTransformator	// is "CTransformer" better?
{
public:
	CTransformator();
	virtual void PushTransformation(const CRenderConfig * ATransformation);
	virtual void PopTransformation();
	virtual void ClearTransformation();
	const CTransformation& GetCurrentTransfomation() const;

protected:
	CTransformation CurrentTransformation;
	vector<CTransformation> TransformationStack;
};

/**
*	CAbstractRenderer - some abstract interface to actual renderer.
*	There are two renderers planned:
*		Fixed Function Pipeline Renderer
*		Programmable Function Pipeline Renderer
*/
class CAbstractRenderer
{
public:
	virtual ~CAbstractRenderer()
	{
		// = 0;
	}
	virtual bool Initialize(){return false;};// = 0;
	virtual bool Finalize(){return false;};// = 0;
	virtual void PushModel(CRenderConfig *, CModel *){};// = 0;
	virtual void Render(){};// = 0;
	virtual void Clear() = 0;
};

/**
*	CFFPRenderer - Fixed Function Pipeline Renderer
*	For compatibility with old machines with no Shaders v3.0
*/
class CFFPRenderer : public CAbstractRenderer
{
public:
	CFFPRenderer();
	~CFFPRenderer();
	bool Initilize();
	bool Finalize();
	void PushModel(CRenderConfig *ARenderInfo, CModel * AModel);
	void Render();
	void Clear();

private:
	class CBetterVertexHolder // For primitives for now
	{
	public:
		CBetterVertexHolder();	// Desired model type;
		virtual ~CBetterVertexHolder();
		virtual void PushVertex(const Vector3 &AVertex, const RGBAf &AColor);
		virtual void RenderPrimitive(GLuint);
		void Clear();
		unsigned int GetVertexCount();

	protected:
		static const unsigned int StartSize = 256;
		unsigned int VertexCount;
		unsigned int ReservedCount;
		RGBAf *Colors;
		Vector3 *Vertices;

		virtual void _Grow();
	};

	class CBetterTextureVertexHolder // For primitives for now
	{
	public:
		CBetterTextureVertexHolder();	// Desired model type;
		virtual ~CBetterTextureVertexHolder();
		virtual void PushVertex(const Vector3 &AVertex, const RGBAf &AColor, const Vector2 &ATexCoord);
		virtual void RenderPrimitive(GLuint);
		void Clear();
		unsigned int GetVertexCount();

	protected:
		static const unsigned int StartSize = 256;
		unsigned int VertexCount;
		unsigned int ReservedCount;
		RGBAf *Colors;
		Vector3 *Vertices;
		Vector2 *TexCoords;

		virtual void _Grow();
	};

	CBetterVertexHolder PrimitiveHolders[MODEL_TYPE_TRIANGLES + 1];
	vector<CBetterTextureVertexHolder*> TexturedGeometry; // per texture i.e. one texture => single DIP
	vector<GLuint> TexIDs;
};

/**
*	CFont - is a resource for font representation;
*	@todo: read from XML, not binary format.
*/
#define CFONT_MAX_SYMBOLS 256
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

/**
*	CRenderProxy - is some another spike.
*/
class CRenderProxy : public CRenderable
{
private:
	CRenderable *RenderSource;

public:
	CRenderProxy(CRenderable *ARenderSource);
	void Render();
};

/**
*	Old bad class, somehow working, but should be reviewed.	
*/
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

/**
*	CFontManager - manages fonts;
*	except the functinality to provide a default font is no different from container of fonts;
*	So if we somehow manage to put this functiality somehere else we'll be able to get rid of that class.
*/
class CFontManager : public CCommonManager <list <CFont*> >, public CTSingleton <CFontManager>
{
public:
	void Init();
	CFont* GetDefaultFont();
	CFont* GetFont(const string &fontname);

protected:	
	CFontManager();
	~CFontManager();
	friend class CTSingleton<CFontManager>;

private:
	CFont *DefaultFont;
};

class CText;	// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
class CGrRect;
class CGrPoint;
class CGrLine;

/**
*	CRenderManager - some monster which definitely can't be eliminated.
*	Manages all stuff for drawing stuff. Traverse tree, applying transformations,
*	Calls renderer to render some stuff...etc
*/
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
	void TransfomationTraverse(CGameObject *Next);

public:
	CCamera	Camera;
	CTransformator Transformator;
	~CRenderManager();
	bool DrawObjects();
	void Print(const CText *Text, const string &Characters);
	void DrawLinedBox(const CRenderConfig* RenderInfo, const CBox &Box);
	void DrawSolidBox(const CRenderConfig* RenderInfo, const CBox &Box);
	void DrawTexturedBox(const CRenderConfig* RenderInfo, const CBox &Box, CTexture *Texture, const Vector2Array<4> &TexCoords);
	void DrawPoint(const CRenderConfig *RenderInfo, const Vector2 &Point);
	void DrawLine(const CRenderConfig *RenderInfo, const Vector2 &v0, const Vector2 &v1);
	void DrawTriangles(const CRenderConfig *RenderInfo, const Vector2 *Vertices, unsigned int Count);
};

#if defined(_WIN32)
	#define SWAP_INTERVAL_PROC PFNWGLSWAPINTERVALFARPROC 
#elif defined(__linux)
	#define SWAP_INTERVAL_PROC PFNGLXSWAPINTERVALSGIPROC
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

typedef int (APIENTRY *SWAP_INTERVAL_PROC)(int);
void setVSync(int interval = 1);

/**
*	CGLWindow Provides functionality for creating window and holding info about window.
*	Why we have this class if we can't create multiple windows, can't change some meaningful
*	window properties in runtime? I think only to have access to window Width() and Height()
*	if so, then this class can easily can be eliminated.
*/
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

/**
*	Stuff beyond this line is now CModel resource and should be eliminated	
*/
class CBasePrimitive : public CRenderConfig
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

/**
*	CText - Text. Cleat enough, lol.
*	Shouldn't render itself, should be inherited from CRenderableComponent, not Renderable
*	So it will have Model and Configuration and there will become some unclear stuff.
*	Cause CModel wants to be Resource.
*/
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

/**
*	CAbstractScene - common interface to Scene. There are two of them - Common Scene and
*	Global Scene // replace scene with sense, lol.
*/
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

/**
*	CSene	
*/
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

/**
*	CGlobalScene
*/
class CGlobalScene : public CScene
{
public:
	void AddRenderable(CRenderable *AObject);
	void AddUpdatable(CUpdatable *AObject);
};

/**
*	CSceneManager
*/
class CSceneManager : CCommonManager <list<CScene*> >, public CTSingleton<CSceneManager>
{
	friend class CTSingleton<CSceneManager>;
public:	
	CAbstractScene* CreateScene();
	void DestroyScene(CAbstractScene *AScene);
	CAbstractScene* GetCurrentScene();
	void SetCurrentScene(CAbstractScene *AScene);
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

/**
*	CMouseCursor	
*	@todo: <strike>make CMouseCusor a separate class</strike>, draw at topmost layer,
*		add possibility to load textured cursors, etc.
*	other @todo: inherit from CRenderComponent and then add to global root object
*		make update position throug events. Compllicity for nothing, as i call it.
*/
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
