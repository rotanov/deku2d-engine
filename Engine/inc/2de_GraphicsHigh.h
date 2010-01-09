#ifndef _2DE_GRAPHICS_HIGH_H_
#define _2DE_GRAPHICS_HIGH_H_

#include "2de_GraphicsLow.h"
#include "2de_MathUtils.h"


//	Глобальные отладочные флаги
// #define _DEBUG_DISABLE_PARTICLES_DRAW
// #define _DEBUG_DISABLE_PARTICLES_UPDATE

//////////////////////////////////////////////////////////////////////////
//Scene manager
//Should be singleton
//Also, should be container In case of Scene manager O(N^2) will be enough for sure, i think.
class CScene : public CObject
{
public:
protected:
private:
};

class CSceneManager : public CList, public CRenderObject, public CUpdateObject
{
public:

private:
protected:
};


//////////////////////////////////////////////////////////////////////////
//Primitive Render

#define G_POLY_TEX_CELL_SIZE 20				//	Размер клетки текстуры шахматной доски для многоугольников.
#define G_POLY_OUTLINE_ENABLE			
#define G_POLY_TEXTURE_ENABLE

#define PRM_RNDR_OPT_BLEND_ONE		0x01	// TODO: нормальные названия для констант
#define PRM_RNDR_OPT_BLEND_OTHER	0x02
#define PRM_RNDR_OPT_BLEND_NO_ONE	0x03

class CPrimitiveRender // Унсаследовать? Но зачем?
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
	*	Внизу функции для отображения примитивов.
	*	Суффикс L - примитив рисуется линиями
	*	S - solid
	*	C - complete
	*	Последнее - по разному для каждого примитива.
	*	Обычно - вращение + линии + заливка + ещё что-то
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
	RGBAf		color;					// цвет
	GLuint		m_textureID;			// опенгловская хрень идентиф. текстуру

	unsigned long	ellapsedtime, lasttime;	// херь чтобы делать время между кадрами
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

//////////////////////////////////////////////////////////////////////////
//CParticleSystem

struct CParticle
{
	Vector2			p;			//	позиция частицы
	Vector2			pp;			//	Сейчас не задействована, но в будующем пригодится - предыдущая позиция. (сейчас она даже не обновляется)
	Vector2			v;			//	скорость 
	RGBAf			c;			//	текущий цвет
	RGBAf			dc;			//	приращение цвета на каждом шаге
	float			size;		//	текущий размер
	float			dsize;		//	приращение размера
	float			life;		//	время жизни частицы; -1 значит бесконечность
	float			age;		//	возраст частицы, то есть сколько она уже просуществовала
};

struct CPsysteminfo
{
	Vector2			p;						//	позиция системы частиц

	RGBAf			sc;						//	начальный цвет каждой частицы
	RGBAf			ec;						//	конечный цвет каждой частицы
	RGBAf			vc;						//	фактор случайности при выборе цвета

	int				MaxParticles;			//	максимально возможно число частиц
	int				ParticlesActive;		//	сколько частиц активно сейчас
	int				lifemin;				//	минимальное время жизни частицы
	int				lifemax;				//	максимальное время жизни частицы

	float			startsize;				//	начальный размер каждой частицы
	float			endsize;				//	конечный размер
	float			sizevar;				//	фактор случайности при выборе размера частицы

	float			plife;					//	авотхуйегознает
	int				plifevar;				//	авотхуйегознает

	float			life;					//	время жизни системы; -1 значит бесонечность
	int				age;					//	текущий возраст системы

	int				emission;				//	фактор, определяющий сколько частиц вылетит за раз
	int				notcreated;				//	количество частиц, которые мы не смогли выпустить в этом кадре по каким-то причинам
	Vector2			*geom;					//	массив точек, для генерации частиц
	int				GeomNumPoints;			//	число этих точек
};

typedef void (*FCreateFunc)(CParticle *);
typedef void (*FUpdateFunc)(CParticle *, float);

/**
*	TODO:
*		1. Добавить интерполяцию покруче, окоромя линейной
*		2. Добавить установку экзмепляра объекта для рендера
*		3. Добавить ещё что-нибуть
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
*	Поскольку Gui петра хуй попользуешься, так как он развёз какую-то еботу 
*	и не может доделать её уже год (сейчас август 09го), то тут будет мини гуи;
*	это будет маленький и удобный и, я надеюсь, временный набор классов. Пока что
*	будет Edit, Button и менеджер. Если будет пиздец, то буду его расширять, появятся табы
*	и всякие свистоперделки жизнено необходимые.
*/

/**
 * CGUIStyle - класс, представляющий стиль GUI, содержащий цвета и размеры стандартных элементов GUI.
 *
 * Для создания нового стиля можно либо наследовать и перегрузить конструктор, либо инстанцировать и изменить нужные значения.
 */

// небольшой комментарий: возможно, кому-то не понравится эта идея, возможно, кто-то любит разукрашивать разные кнопки в разные цвета - и т. п.
// 			  я предлагаю такое решение: у каждого элемента будет указатель на стиль
// 			  поэтому, можно будет создать по стилю на каждую кнопку и указать его
// 			  или же наследовать кнопку и перегрузить в ней конструктор, который будет создавать определённый стиль
// 			  вобщем - гибкость рулит, но и унификация стиля - хорошая вещь

class CGUIStyle
{
public:
	struct CGUIStyleColors
	{
		RGBAf FocusRect;
		RGBAf ButtonFace;
		RGBAf ButtonFaceHovered;
		RGBAf ButtonFacePressed;
		RGBAf ButtonBorder;
		RGBAf ButtonBorderHovered;
		RGBAf ButtonBorderPressed;
		RGBAf ButtonText;
		RGBAf ButtonInactiveText;
		RGBAf EditBackground;
		RGBAf EditBackgroundHovered;
		RGBAf EditBorder;
		RGBAf EditBorderHovered;
		RGBAf EditText;
		RGBAf EditInactiveText;
		RGBAf EditSelection;
	};
	struct CGUIStyleMetrics
	{
		scalar FocusRectSpacing;
		scalar FocusRectLineWidth;
		Vector2 EditMargins;
		scalar EditBorderWidth;
	};

	CGUIStyle()
	{
		// default style values - very ugly style :) I'm programmer, not fucking "эстет" :) you're welcome to fix colors to more beautiful ones

		Colors.FocusRect = RGBAf(0.5f, 0.5f, 0.5f, 1.0f);
		Colors.ButtonFace = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.ButtonFaceHovered = RGBAf(0.6f, 0.6f, 0.6f, 1.0f);
		Colors.ButtonFacePressed = RGBAf(0.45f, 0.45f, 0.45f, 1.0f);
		Colors.ButtonFacePressed = RGBAf(0.45f, 0.45f, 0.45f, 1.0f);
		Colors.ButtonBorder = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.ButtonBorderHovered = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.ButtonBorderPressed = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.ButtonText = COLOR_BLACK;
		Colors.ButtonInactiveText = RGBAf(0.2f, 0.2f, 0.2f, 1.0f);
		Colors.EditBackground = COLOR_WHITE;
		Colors.EditBackgroundHovered = COLOR_WHITE;
		Colors.EditBorder = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.EditBorderHovered = RGBAf(0.75f, 0.75f, 0.75f, 1.0f);
		Colors.EditText = COLOR_BLACK;
		Colors.EditInactiveText = RGBAf(0.2f, 0.2f, 0.2f, 1.0f);
		Colors.EditSelection = RGBAf(0.0f, 0.4f, 0.8f, 0.5f);

		Metrics.FocusRectSpacing = 5.0f;
		Metrics.FocusRectLineWidth = 0.5f;
		Metrics.EditMargins = Vector2(4.0f, 6.0f);
		Metrics.EditBorderWidth = 2.0f;
	}
	CGUIStyleColors Colors;
	CGUIStyleMetrics Metrics;

	// TODO: loading style from XML file (and even maybe saving)
};

class CGUIObjectBase : public CRenderObject, public CUpdateObject
{
public:
	struct CMouseState
	{
		bool Hovered;
		bool Pressed;
		bool PressedInside;
		bool PressedOutside;
	};
	string				Text;
	Callback			CallProc;			//	Указатель на пользовательскую коллбэк ф-ю, будет вызываться для каждого наследника по своему

	CGUIObjectBase();
	CGUIObjectBase(CGUIObjectBase *AParent);
	~CGUIObjectBase();

	void	SetFont(CFont *AFont);
	void	SetPrimitiveRender(CPrimitiveRender *APrimitiveRender);
	void	SetParent(CGUIObjectBase *AParent);
	Vector2 GlobalToLocal(const Vector2& Coords) const;
protected:
				// вот зачем эти очвевидные комментарии? неужели кому-то не понятно, что CFont *Font - это указатель на шрифт?
				// нет, блядь, это наверное число ядерных распадов на Солнце с момента создания объекта....
				//	Я оставил их тут, потому что у меня есть хитрый план. Непосвящённые не знают.
	CFont				*Font;				//	Указатель на шрифт.
	CPrimitiveRender	*PRender;			//	Указатель на рендер примитивов.
	CGUIObjectBase			*Parent;			//	Указатель на родительский объект. На будущее; иерархии виджетов пока нет
	CMouseState MouseState;
	CMouseState PreviousMouseState;
	CMouseState WidgetState;
};


class CGUIObject : public CGUIObjectBase
{
public:
	CGUIObject();

	bool isFocused() const;
	void Focus();
	
	CGUIStyle* GetStyle() const { return Style; }
	void SetStyle(CGUIStyle *AStyle) { Style = AStyle; }

protected:
	CGUIStyle *Style;
};


class CGUIManager : public CList, public CGUIObjectBase, public CTSingleton<CGUIManager>
{
public:
				~CGUIManager();
	bool		InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter);
	bool		Update(scalar dt);
	bool		Render();
	CGUIObject* GetFocusedObject() const;
	void		SetFocusedNodeTo(CListNode *AFocusedNode);
	void		SetFocus(CObject *AObject);
	CGUIStyle*	GetStyle() { return &Style; }
	void		SetStyle(const CGUIStyle &AStyle) { Style = AStyle; }
private:
	int			KeyHoldRepeatDelay;				// множественный костыль! TODO: fix
	CListNode	*FocusedOnListNode;
	CGUIObject	*FocusedOn;
	int			KeyHoldRepeatInterval;
	int			TimerAccum;
	bool		tabholded;
	bool		repeatstarted;
	CGUIStyle	Style;
protected:
	CGUIManager();
	friend class CTSingleton<CGUIManager>;
};

class CLabel : public CGUIObject
{
public:
	CLabel();
	CLabel(const string &AText)
	{
		Text = AText;
		Font = CFontManager::Instance()->GetFont("Font");
	}
	bool Render();
	bool Update(float dt)
	{
		return true;
	}
};

class CButton : public CGUIObject
{
public:
				CButton();
				CButton(CAABB ARect, const char* AText, RGBAf AColor, Callback ACallProc);
				~CButton();
	bool		InputHandling(Uint8 state, Uint16 key, SDLMod, char letter);
	bool		Render();
	bool		Update(float dt);
};


class CEdit : public CGUIObject
{
public:
	class CTextSelection
	{
	public:
		int Start;
		int End;
		CTextSelection();
		CTextSelection(int AStart, int AEnd);
		void Set(int AStart, int AEnd);
		bool Exists() const;
		int RangeStart() const;
		int RangeEnd() const;
		int Length() const;
		void Clear();
		void Clear(int ACursorPos);
	};
				CEdit();
				~CEdit();
	bool		InputHandling(Uint8 state, Uint16 key, SDLMod, char letter);
	bool		Render();
	bool		Update(scalar dt);
private:
	int		CursorPos;
	int		MouseToCursorPos(const Vector2& MousePosition);
	string		GetVisibleText() const;
	bool		isTextFits(const char *AText) const;
	int		VisibleTextOffset;
	CTextSelection	Selection;
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
