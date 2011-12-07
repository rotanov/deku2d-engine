#ifndef _2DE_GRAPHICS_LOW_H_
#define _2DE_GRAPHICS_LOW_H_

#include "2de_Core.h"
#include "2de_ImageUtils.h"
//#include "2de_Math.h"
#include "2de_Resource.h"
#include "2de_Vector2.h"
#include "2de_Vector3.h"
#include "2de_Box.h"
#include "2de_RGBA.h"

namespace Deku2D
{
	#define _2DE_DEBUG_DRAW_BOXES // wtf? I found no other occurrences

	// OpenGL is not included in the interface now, so I'll redefine it myself
	typedef unsigned GLuint;
	typedef unsigned GLenum;
	typedef int GLsizei;

	//////////////////////////////////////////////////////////////////////////
	// Various constants

	namespace Const
	{
		namespace Color
		{
			const RGBAf WHITE = RGBAf(1.00f, 1.00f, 1.00f, 1.00f);
			const RGBAf BLACK = RGBAf(0.00f, 0.00f, 0.00f, 0.00f);
			const RGBAf RED	= RGBAf(0.98f, 0.05f, 0.01f, 1.00f);
			const RGBAf GREEN = RGBAf(0.10f, 0.90f, 0.05f, 1.00f);
			const RGBAf BLUE = RGBAf(0.01f, 0.15f, 0.85f, 1.00f);

		}	//	namespace Color
		
		namespace Graphics
		{
			const Vector2 V2_QuadBin[4] = // Four vectors representing the quad with vertices (0 0) (1 0) (1 1) (0 1)
			{
				Math::V2_ZERO,
				Math::V2_RIGHT,
				Math::V2_RIGHT + Math::V2_UP,
				Math::V2_UP,
			};

			const Vector2 V2_QuadBinCenter[4] = // Four vectors representing the quad with vertices (-1 -1) (1 -1) (1 1) (-1 1)
			{
				Math::V2_LEFT + Math::V2_DOWN,
				Math::V2_RIGHT + Math::V2_DOWN,
				Math::V2_RIGHT + Math::V2_UP,
				Math::V2_LEFT + Math::V2_UP,
			};

			const Vector2Array<4> V2_QUAD_BIN =  Vector2Array<4>(V2_QuadBin);
			const Vector2Array<4> V2_QUAD_BIN_CENTER = Vector2Array<4>(V2_QuadBinCenter);

			const float ROTATIONAL_AXIS_Z = 1.0f;

			extern unsigned  IMAGE_SHROOM_TITLE_SIZE;
			extern char IMAGE_SHROOM_TITLE_DATA[];

			extern const unsigned BINARY_DATA_DEFAULT_FONT_TEXTURE_SIZE;
			extern char BINARY_DATA_DEFAULT_FONT_TEXTURE[];

			extern const unsigned BINARY_DATA_DEFAULT_FONT_SIZE;
			extern char BINARY_DATA_DEFAULT_FONT[];

		}	//	namespace Graphics

	}	//	namespace Const

	class CAbstractScene;

	/**
	*	CTexture is like CGLImageData, but also has CResource functionality
	*	can Load() and Unload() and even SaveToFile(). Wait, it still can't
	*	save to file, but function is defined @todo: implement saving of textures to
	*	file. And don't even try to think when we'll need such possibility
	*	@todo: Look at loading issue, described in MakeTexture implementation
	*	Make able to save texture to file if it was unloaded from memory
	*	hint: use glGetTexture or glTexSubImage2D
	*/

	class CTexture : public CImageData, public CResource
	{
	public:
		CTexture();
		virtual ~CTexture();

		bool Load();
		void Unload();

		bool MakeTexture();

		/**
		*	As I think for now this one should save Texture to file.
		*	I mean we can render something to Texture, and then save it
		*	as image on HDD; The trivial example is screenshoot.
		*	Less trivial is some intermediate textures for gfx;
		*/
		bool SaveToFile(const string &AFilename);
		GLuint GetTexID() const;

	private:
		GLuint TexID;
		bool doCleanData;
	};

	/**
	*	CTextureManager — actually a useless class used as a container for textures;
	*	in order to access texture; So @todo: get rid of CTextureManager
	*/

	class CTextureManager : public CCommonManager <vector <CTexture*> >
	{
	public:
		bool UnloadTextures();

	protected:
		CTextureManager();
		friend class CTSingleton <CTextureManager>;
	};

	static CTSingleton<CTextureManager> TextureManager;

	/**
	*	CTransformation - class describes placement in space and some other properties
	*	which affects to object position, orientation and size;
	*	@todo: access to transformation matrices
	*/

	class CTransformation
	{
	private:
		float DepthOffset;
		Vector2 Translation;
		float Rotation;
		Vector2 Scaling;
		

	public:
		CTransformation(float ADepthOffset = 0.0f, const Vector2 &ATranslation = Const::Math::V2_ZERO,
			float ARotation = 0.0f, const Vector2 &AScaling = Const::Math::V2_TOP_RIGHT);
		/**
		*	Don't be confused here: Look to implementation - it applies one transformation
		*	on other to represent common transformation, not just add members
		*/
		CTransformation& operator +=(const CTransformation &rhs);

		float GetRotation() const;
		float GetDepth() const;
		const Vector2& GetScaling() const;
		const Vector2& GetTranslation() const;
		
		void SetRotation(float ARotation);
		void SetDepth(float ADepth);
		void SetScaling(const Vector2 &AScaling);
		void SetTranslation(const Vector2 &ATranslation);

		Vector2 Apply(const Vector2 &AVector);
		CBox Apply(const CBox &ABox);

		void Clear();
	};

	/**
	* CVertexHolder - incorporates the ability to hold vertices, colors, tc's and work with 'em.
	*/
	template <typename TypeVertices>
	class CVertexHolder
	{
	public:
		CVertexHolder(bool AHasTexCoords) : HasTexCoords(AHasTexCoords), VertexCount(0), ReservedCount(START_SIZE), TexCoords(NULL)
		{
			Colors = new RGBAf [START_SIZE];
			Vertices = new TypeVertices [START_SIZE];	
			if (HasTexCoords)
				TexCoords = new Vector2[START_SIZE];
		}
		~CVertexHolder()
		{
			delete [] Colors;
			delete [] Vertices;
			if (HasTexCoords)
				delete [] TexCoords;
		}
		void ToggleTextureSupport(bool ASupportTexture)
		{
			if (ASupportTexture == HasTexCoords)
				return;
			HasTexCoords = ASupportTexture;
			if (HasTexCoords)
			{
				TexCoords = new Vector2[ReservedCount];
			}
			else
			{
				SAFE_DELETE_ARRAY(TexCoords);
				TexCoords = NULL;
			}
		}
		void PushVertex(const TypeVertices &AVertex, const RGBAf &AColor, const Vector2 &ATexCoord)
		{
			assert(HasTexCoords);
			if (VertexCount == ReservedCount)
			{
				ReservedCount = VertexCount * 2;
				Grow();
			}
			Vertices[VertexCount] = AVertex;
			Colors[VertexCount] = AColor;
			TexCoords[VertexCount] = ATexCoord;
			VertexCount++;
		}
		void PushVertex(const TypeVertices &AVertex, const RGBAf &AColor)
		{
			assert(!HasTexCoords);
			if (VertexCount == ReservedCount)
			{
				ReservedCount = VertexCount * 2;
				Grow();
			}
			Vertices[VertexCount] = AVertex;
			Colors[VertexCount] = AColor;
			VertexCount++;
		}
		void RenderPrimitive(GLuint AType) const;
		void Clear()
		{
			VertexCount = 0;
		}
		unsigned GetVertexCount() const
		{
			return VertexCount;
		}
		void Reserve(unsigned AVertexCount)
		{
			if (AVertexCount <= ReservedCount)
				return;
			ReservedCount = AVertexCount;
			Grow();
		}
		const TypeVertices* GetVertices() const
		{
			return Vertices;
		}
		const Vector2* GetTexCoords() const
		{
			return TexCoords;
		}
		TypeVertices* GetVertices()
		{
			return Vertices;
		}
		Vector2* GetTexCoords()
		{
			return TexCoords;
		}

	protected:
		static const unsigned START_SIZE = 256;
		bool HasTexCoords;
		unsigned VertexCount;
		unsigned ReservedCount;
		RGBAf *Colors;
		TypeVertices *Vertices;
		Vector2 *TexCoords;

		void Grow()
		{
			RGBAf *NewColors = new RGBAf[ReservedCount];
			for (unsigned i = 0; i < VertexCount; i++)
				NewColors[i] = Colors[i];
			delete[] Colors;
			Colors = NewColors;

			TypeVertices *NewVertices = new TypeVertices[ReservedCount];
			for (unsigned i = 0; i < VertexCount; i++)
				NewVertices[i] = Vertices[i];
			delete[] Vertices;
			Vertices = NewVertices;

			if (HasTexCoords)
			{
				Vector2 *NewTexCoords = new Vector2[ReservedCount];
				for (unsigned i = 0; i < VertexCount; i++)
					NewTexCoords[i] = TexCoords[i];
				delete [] TexCoords;
				TexCoords = NewTexCoords;
			}

		}
		CVertexHolder();
	};

	/**
	*	CModel — represents object geometry, object type and if has - 
	*	texture and texture coordinates. 
	*/

	enum EModelType
	{
		MODEL_TYPE_NOT_A_MODEL = -1,
		MODEL_TYPE_POINTS = 1,
		MODEL_TYPE_LINES = 2,
		MODEL_TYPE_TRIANGLES = 3,
	};

	class CModel : public CResource
	{
	public:
		CModel(EModelType AModelType = MODEL_TYPE_NOT_A_MODEL, CTexture * ATexture = NULL,
			unsigned AVerticesNumber = 0, Vector2* AVertices = NULL, 
			Vector2* ATexCoords = NULL);
		~CModel();
		void SetTexture(CTexture *ATexture);
		void SetModelType(EModelType AModelType);
		CTexture* GetTexture() const;
		const Vector2* GetVertices() const;
		const Vector2* GetTexCoords() const;
		EModelType GetModelType() const;
		virtual int GetVertexNumber() const;
		const RGBAf* GetColors() const;
		const CBox& GetBox() const;
		bool Load();
		void Unload();

	private:
		CBox Box;
		CResourceRefCounter<CTexture> Texture;
		EModelType ModelType;

	private:
		CVertexHolder<Vector2> Vertices;
		void UpdateBox();
	};

	/**
	*	Configuration of object, that represents object transformation
	*	and some render options.
	*/

	enum EBlendingMode
	{
		BLEND_MODE_OPAQUE = 0,
		BLEND_MODE_TRANSPARENT = 1,
		BLEND_MODE_ADDITIVE = 2,
	};

	class CRenderConfig
	{
	public:
		bool doIgnoreTransform; 
		RGBAf Color;
		EBlendingMode BlendingMode;

		CRenderConfig();
		EBlendingMode GetBlendingMode() const;
		void SetBlendingMode(EBlendingMode ABlendingMode);
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
		// vector<CGeometry> Primitives; in order to not include one more header for draft not used class
		vector<string> Tags;
	};

	class CFrameSequence
	{
	public:
		vector<CFrameInfo> Rectangles;

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
		virtual void PushTransformation(const CTransformation &ATransformation);
		virtual void PopTransformation();
		virtual void ClearTransformation();
		const CTransformation& GetCurrentTransfomation() const;

		bool doConsiderCamera; // Shows if we considering global camera at current depth in trans. tree
		bool isMirroredHorizontal;
		bool isMirroredVertical;

		vector<CTransformation> TransformationStack;
	protected:
		CTransformation CurrentTransformation;
		
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
		virtual void PushModel(const CTransformation &, const CRenderConfig *, const CModel *){};// = 0;
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
		bool Initialize();
		bool Finalize();
		void PushModel(const CTransformation& ATransform, const CRenderConfig *ARenderInfo, const CModel * AModel);
		void Render();
		void Clear();

	private:

	#define TOTAL_HOLDERS (MODEL_TYPE_TRIANGLES * (BLEND_MODE_ADDITIVE + 1))	// no way that could be nice
		CVertexHolder<Vector3> *PrimitiveHolders[TOTAL_HOLDERS];	// 9 total
		map<CTexture*, CVertexHolder<Vector3>*> texturedGeometry[3];
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
		float SymbolWidth(unsigned Index) const;
		Vector2Array<4> GetTexCoords(unsigned Charachter);
		//const Vector2Array<4>& GetTexCoords(unsigned Charachter)
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

	class CFontManager : public CCommonManager <list <CFont*> >
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

	static CTSingleton<CFontManager> FontManager;

	/**
	*	CRenderManager - some monster which definitely can't be eliminated.
	*	Manages all stuff for drawing stuff. Traverse tree, applying transformations,
	*	Calls renderer to render some stuff...etc
	*/

	class CText;
	class CRenderableComponent;

	class CRenderManager : public CObject /*, public CCommonManager <std::vector <CRenderableComponent*> >*/
	{
	protected:
		CRenderManager();
		friend class CTSingleton <CRenderManager>;
		friend class CEngine;
		friend class CDrawVisitor;

	private:
		CAbstractRenderer *Renderer;

		void SetBlendingMode();
		void BeginFrame();
		void EndFrame();

	public:
		CCamera	Camera;
		CTransformator Transformator;
		~CRenderManager();
		bool DrawObjects();

		static CModel* CreateModelCircle(float Radius, EModelType AModelType = MODEL_TYPE_LINES, int Precision = 16);
		static CModel* CreateModelBox(float Width, float Height, 
			EModelType AModelType = MODEL_TYPE_LINES, CTexture * ATexture = NULL,
			const Vector2Array<4> &ATexCoords = Const::Graphics::V2_QUAD_BIN);
		static CModel* CreateModelLine(const Vector2 &v0, const Vector2 &v1);
		static CModel* CreateModelText(const CText *AText);

		static EModelType SelectModelTypeByStringIdentifier(const string &Identifier);
		static EBlendingMode SelectBlendingModeByIdentifier(const string &Identifier);
		static string StringIdentifierByModelType(EModelType AModelType);
		
		void SetSwapInterval(int interval = 1);
	};

	static CTSingleton<CRenderManager> RenderManager;

	/**
	* CGLWindow - a class that represents a GL window, is mostly used to create and resize it, and also holds video-mode-related things.
	*/

	class CGLWindow : public CObject
	{
	public:
		struct WindowVideoParameters	// Where is fucking "C"? "struct" is deprecated by our docs. I missed this with variable, really.
		{
			unsigned Width;
			unsigned Height;
			byte BPP;
			bool isFullscreen;

			bool operator==(const WindowVideoParameters &rhs) const;
			bool operator!=(const WindowVideoParameters &rhs) const;
			string GetString() const;
		};

		bool Create(int AWidth, int AHeight, byte ABPP, bool AFullscreen, const string &ACaption);
		bool Create();

		bool Initialize();
		void GLInit();

		unsigned GetWidth() const;
		unsigned GetHeight() const;
		Vector2 GetSize() const;
		bool GetBPP() const;
		bool GetFullscreen() const;
		string GetCaption() const;
		RGBAf GetBackgroundColor() const;

		void SetWidth(unsigned AWidth);
		void SetHeight(unsigned AHeight);
		void SetSize(unsigned AWidth, unsigned AHeight);
		void SetBPP(byte ABPP);
		void SetFullscreen(bool AFullscreen);
		void SetCaption(const string &ACaption);
		void SetBackgroundColor(const RGBAf &AColor);
		void Resize(unsigned AWidth, unsigned AHeight);

		WindowVideoParameters GetVideoMode() const;
		void SetVideoMode(const WindowVideoParameters &AVideoMode);

		WindowVideoParameters GetDesktopVideoMode() const;

	protected:
		CGLWindow();
		friend class CTSingleton<CGLWindow>;

	private:
		WindowVideoParameters Parameters;
		WindowVideoParameters NewParameters;
		WindowVideoParameters Desktop;

		string Caption;
		RGBAf BackgroundColor;
		bool isCreated;
	};

	static CTSingleton<CGLWindow> GLWindow;

	/**
	*	CAbstractScene - common interface to Scene. There are two of them - Common Scene and
	*	Global Scene
	*/

	class CAbstractScene : public CObject
	{
		friend class CSceneManager;
	public:
		virtual void Add(CGameObject *AObject) = 0;
		virtual void Remove(CGameObject *AObject) = 0;

	protected:

		CAbstractScene();
		~CAbstractScene(){}

	private:
		CAbstractScene(const CAbstractScene &AScene);
		CAbstractScene& operator=(const CAbstractScene &AScene);
	};

	/**
	*	CScene	
	*/

	class CScene : public CAbstractScene
	{
		friend class CSceneManager;
	public:
		virtual void Add(CGameObject *AObject);
		virtual void Remove(CGameObject *AObject);

	protected:
		vector<CGameObject *> GameObjects;

		CScene()
		{
			ClassName = "Scene";
		}
		~CScene();
	};

	/**
	*	CGlobalScene
	*/

	class CGlobalScene : public CScene
	{
	public:
		void Add(CGameObject *AObject);	

	};

	/**
	*	CSceneManager
	*/

	class CSceneManager : CCommonManager <list<CScene*> >
	{
		friend class CTSingleton<CSceneManager>;
	public:	
		CAbstractScene* CreateScene();
		void DestroyScene(CAbstractScene *AScene);
		CAbstractScene* GetCurrentScene();
		void SetCurrentScene(CAbstractScene *AScene);
		bool InScope(CAbstractScene * AScene) const;

	private:
		CAbstractScene *CurrentScene;
		CGlobalScene GlobalScene;
		vector<CAbstractScene*> Scenes;

	protected:
		~CSceneManager();
		CSceneManager();
	};

	static CTSingleton<CSceneManager> SceneManager;

}	//	namespace Deku2D

#endif // _2DE_GRAPHICS_LOW_H_
