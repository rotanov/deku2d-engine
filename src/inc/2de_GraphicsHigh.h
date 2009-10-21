#ifndef _2DE_GRAPHICS_HIGH_H_
#define _2DE_GRAPHICS_HIGH_H_

#include "2de_GraphicsLow.h"
#include "2de_MathUtils.h"


//	���������� ���������� �����
// #define _DEBUG_DISABLE_PARTICLES_DRAW
// #define _DEBUG_DISABLE_PARTICLES_UPDATE

//////////////////////////////////////////////////////////////////////////
//Primitive Render

#define G_POLY_TEX_CELL_SIZE 20				//	������ ������ �������� ��������� ����� ��� ���������������.
#define G_POLY_OUTLINE_ENABLE			
#define G_POLY_TEXTURE_ENABLE

#define PRM_RNDR_OPT_BLEND_ONE		0x01	// TODO: ���������� �������� ��� ��������
#define PRM_RNDR_OPT_BLEND_OTHER	0x02
#define PRM_RNDR_OPT_BLEND_NO_ONE	0x03

class CPrimitiveRender // �������������? �� �����?
{
public:
	int BlendingOption, CheckerCellSize;
	bool OutlineEnabled, TextureEnabled, DashedLines, doUseGlobalCoordSystem, LineStippleEnabled;
	static int glListCircleL;
	static int glListCircleS;
	static int glListQuarterCircle;
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
		Angle = depth = 0.0f;
		lwidth = 1.0f;
		psize = 8.0f;
		LineStippleEnabled = doUseGlobalCoordSystem = false;			
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

	void grQuarterCircle(const Vector2 &v0, scalar Radius);

	void grCircleL(const Vector2 &p, scalar Radius);
	void grCircleS(const Vector2 &p, scalar Radius);
	void grCircleC(const Vector2 &p, scalar Radius);

	void grPolyC(const Vector2 &p, scalar angle, CPolygon *poly);

	void grRingS(const Vector2 &p, scalar Radius);
	void grRingC(const Vector2 &p, scalar Radius);

	void grArrowL(const Vector2& v0, const Vector2& v1);
	void grArrowC(const Vector2 &v0,const Vector2 &v1);

	void gDrawBBox(CAABB box);
	void grInYan(const Vector2 &p, scalar Radius);
private:
	void BeforeRndr();
	void AfterRndr();
	void CheckBlend();
	void CheckTexture();
};

//////////////////////////////////////////////////////////////////////////
//CSprite

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

//////////////////////////////////////////////////////////////////////////
//CParticleSystem

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
	CPsysteminfo			Info;
	CParticle*				particles;
	CTexture*				Texture;

	CRenderObject*			UserRenderSample;
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

	CParticle*				CreateParticle();
	void					SetGeometry(Vector2 * points, int numPoints);
	void					SetUserUpdate(FUpdateFunc func);
	void					SetUserCreate(FCreateFunc func);

	bool					SaveToFile();
	bool					LoadFromFile();

protected:
	void					_swap(int i, int j);
};


//////////////////////////////////////////////////////////////////////////
//Gui 

/**
*	��������� Gui ����� ��� �������������, ��� ��� �� ����� �����-�� ����� 
*	� �� ����� �������� � ��� ��� (������ ������ 09��), �� ��� ����� ���� ���;
*	��� ����� ��������� � ������� �, � �������, ��������� ����� �������. ���� ���
*	����� Edit, Button � ��������. ���� ����� ������, �� ���� ��� ���������, �������� ����
*	� ������ �������������� ������� �����������.
*/

enum EWidgetMouseState {wmsInside, wmsOutside, wmsLost, wmsHovered, wmsClicked, wmsReleased};

class CGUIObject : public CRenderObject, public CUpdateObject
{
public:
	//���� ���� � ������
	//RGBAf				color;				// ����. � ����� ������ ��������� �� ����, ����� ����� ������ ����� ������ ����� ����, ���...��������� ��������. ���� ����� ���. ��� �� ���� ���.
	string				text;				// ��� �������, �����.
	Callback			CallProc;			//	��������� �� ���������������� ������� �-�, ����� ���������� ��� ������� ���������� �� ������
											// ��������� ����������� ����� ���������, ��� ���� ���� ������� ��.
			CGUIObject();
			~CGUIObject();
	void	SetFont(CFont *AFont);
	void	SetPrimitiveRender(CPrimitiveRender *APrimitiveRender);
	void	SetParent(CGUIObject *AParent);
protected:
	CFont				*Font;				//	��������� �� �����.
	CPrimitiveRender	*PRender;			//	��������� �� ������ ����������.
	CGUIObject			*Parent;			//	��������� �� ������������ ������. �� �������; �������� �������� ���� ���
	EWidgetMouseState	WidgetMouseState;	//	��������� ���� ������������ ������� � ������ ������� ���������
};

class CGUIManager : public CList, public CGUIObject
{
public:
				CGUIManager();
				~CGUIManager();
	bool		InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	bool		Update(scalar dt);
	bool		Render();
	void		SetFocusedNodeTo(CListNode* AFocusedNode);
	void		SetFocus(CObject* AObject);
private:
	int			KeyHoldRepeatDelay;				// ������������� �������! TODO: fix
	CListNode	*FocusedOnListNode;
	CGUIObject	*FocusedOn;
	int			KeyHoldRepeatInterval;
	int			TimerAccum;
	bool		tabholded;
	bool		repeatstarted;
};

extern CGUIManager GuiManager;

class CButton : public CGUIObject
{
public:
				CButton();
				CButton(CAABB ARect, char* AText, RGBAf AColor, Callback ACallProc);
				~CButton();
	bool		Render();
	bool		Update(float dt);
};

class CEdit : public CGUIObject
{
public:
				CEdit();
				~CEdit();
	bool		InputHandling(Uint8 state, Uint16 key, SDLMod, char letter);
	bool		Render();
	bool		Update(scalar dt);
private:
	int			CursorPos;
};

class CMenuItem : public CGUIObject, public CList
{
public:
	bool		isCycledMenuSwitch;
				CMenuItem();
				CMenuItem(CMenuItem* AParent, char* AMenuText, Callback ACallProc);
				~CMenuItem();
	bool		Render();
	bool		Update(scalar dt);
	bool		InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	bool		AddObject(CObject *AObject);
private:
	CMenuItem*	FocusedOnItem;
	CListNode*	FocusedOnListNode;
};
//////////////////////////////////////////////////////////////////////////
//CText not implemented yet
class CText : public CRenderObject
{
private:
	void Draw()
	{

	}
};

#endif // _2DE_GRAPHICS_HIGH_H_
