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

class CSceneManager : CCommonManager<CRenderObject>, public CRenderObject, public CUpdateObject
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

#define PRM_RNDR_OPT_BLEND_ONE		0x01	// @todo: нормальные названия для констант
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
	float Angle, lwidth, depth, psize;
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
		BlendingOption = PRM_RNDR_OPT_BLEND_OTHER;
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

	void grQuarterCircle(const Vector2 &v0, float Radius);

	void grCircleL(const Vector2 &p, float Radius);
	void grCircleS(const Vector2 &p, float Radius);
	void grCircleC(const Vector2 &p, float Radius);

	void grPolyC(const Vector2 &p, float angle, CPolygon *poly);

	void grRingS(const Vector2 &p, float Radius);
	void grRingC(const Vector2 &p, float Radius);

	void grArrowL(const Vector2& v0, const Vector2& v1);
	void grArrowC(const Vector2 &v0,const Vector2 &v1);

	void gDrawBBox(CAABB box);
	void grInYan(const Vector2 &p, float Radius);
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
private:
	CTexture	*Texture;

	unsigned long	EllapsedTime;
	unsigned long	LastTime;
	unsigned int	CurrentFrame;
	unsigned int	CurrentRow;
	unsigned int	CurrentColumn;

	bool		isFirstTimeRendering;

	size_t		AnimationsCount;
	SAnimationInfo *animations;
	SAnimationInfo *anim;

public:
	RGBAf		Color;
	bool mirror_h;
	CSprite() : isFirstTimeRendering(true), Color(COLOR_WHITE), animations(NULL),
		anim(NULL), AnimationsCount(0), EllapsedTime(0), LastTime(0), CurrentFrame(0),
		CurrentRow(0), CurrentColumn(0), mirror_h(false)
	{
	}
	~CSprite();
	bool SetTexture(const string &TextureName);
	bool AddAnimation(bool _isAnimated, float _m_fFrameDelay, float _m_fwidth,
		float _m_fheight, int _m_nNumFrameColumns, int _m_nNumFrameRows,
		int _m_nTotalFrames, int _m_nFrameWidth, int _m_nFrameHeight,
		int _m_nOffsetX, int _m_nOffsetY, int _AnimationIndex, bool _isLoop);	
	void Render();
	void SetAnimation(int index);
	SAnimationInfo* FindAnimation(int index);
	bool LoadFromFile(char* Filename);
	bool SaveToFile(char *Filename);
};

//////////////////////////////////////////////////////////////////////////
//CParticleSystem

struct CParticle
{
	Vector2			Position;				//	позиция частицы
	Vector2			PreviousPosition;		//	Сейчас не задействована, но в будующем пригодится - предыдущая позиция. (сейчас она даже не обновляется)
	Vector2			Velocity;				//	скорость 
	RGBAf			Color;					//	текущий цвет
	RGBAf			DeltaColor;				//	приращение цвета на каждом шаге
	float			Size;					//	текущий размер
	float			DeltaSize;				//	приращение размера
	float			Life;					//	время жизни частицы; -1 значит бесконечность
	float			Age;					//	возраст частицы, то есть сколько она уже просуществовала // Была озвучена такая точка зрения:
											//	life можно не хранить, достаточно только выставлять age в нужное значение при создании и уменьшать это значение на
											//	каждом шаге. Сравнивать с нулём. Надо подумать, точно ли нам нигде больше не пригодится Life
};

typedef void (*FCreateFunc)(CParticle *);
typedef void (*FUpdateFunc)(CParticle *, float);

/**
*	@todo:
*		1. Добавить интерполяцию покруче, окоромя линейной
*		2. Добавить установку экзмепляра объекта для рендера
*		3. Добавить ещё что-нибуть
*/

class CParticleSystem :  public CResource, public CRenderObject, public CUpdateObject
{
public:
	CParticle			*Particles;
	Vector2				Position;					//	позиция системы частиц
	Vector2				*PtrPosition;
	

	RGBAf				ColorStart;					//	начальный цвет каждой частицы
	RGBAf				ColorOver;					//	конечный цвет каждой частицы
	RGBAf				ColorVariability;			//	фактор случайности при выборе цвета

	int					MaxParticles;				//	максимально возможно число частиц
	int					ParticlesActive;			//	сколько частиц активно сейчас

	float				SizeStart;					//	начальный размер каждой частицы
	float				SizeOver;					//	конечный размер
	float				SizeVariability;			//	фактор случайности при выборе размера частицы

	float				ParticleLife;				//	авотхуйегознает
	int					ParticleLifeVariability;	//	авотхуйегознает
	
	float				Life;						//	время жизни системы; -1 значит бесонечность
	float				Age;						//	текущий возраст системы
	
	int					Emission;					//	фактор, определяющий сколько частиц вылетит за раз
	int					NotCreated;					//	количество частиц, которые мы не смогли выпустить в этом кадре по каким-то причинам
	Vector2				*GeometryVertices;			//	массив точек, для генерации частиц
	int					GeometryPointsCount;		//	число этих точек

	CTexture			*Texture;

	CRenderObject			*UserRenderSample;
	bool					user_create;
	bool					user_update;
	bool					user_render;

	void					(*procUserCreate)(CParticle *);
	void					(*procUserUpdate)(CParticle *, float);

	CParticleSystem();
	~CParticleSystem();
	void					Init();	
	void					Update(float dt);
	void					Render();

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
//CText not implemented yet
class CText : public CRenderObject
{
public:
	CFont *Font;
	string Text;
	CText();
	void Render();
};
#endif // _2DE_GRAPHICS_HIGH_H_
