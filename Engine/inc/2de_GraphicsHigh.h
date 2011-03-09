#ifndef _2DE_GRAPHICS_HIGH_H_
#define _2DE_GRAPHICS_HIGH_H_

#include "2de_Core.h"
#include "2de_Components.h"
#include "2de_GraphicsLow.h"
#include "2de_Math.h"
#include "2de_Update.h"

/**
*	CSprite	
*/
struct SAnimationInfo				// структура определяющая параметры анимации // хуита
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

class CSprite : public CRenderableComponent	// говно
{
public:
	CSprite();
	~CSprite();
	bool SetTexture(const string &TextureName);
	bool AddAnimation(bool _isAnimated, float _m_fFrameDelay, float _m_fwidth,
		float _m_fheight, int _m_nNumFrameColumns, int _m_nNumFrameRows,
		int _m_nTotalFrames, int _m_nFrameWidth, int _m_nFrameHeight,
		int _m_nOffsetX, int _m_nOffsetY, int _AnimationIndex, bool _isLoop);
	void Render();
	void SetAnimation(int index);
	SAnimationInfo* FindAnimation(int index);
	bool LoadFromFile(const string &Filename);
	bool SaveToFile(const string &Filename);

	RGBAf		Color;
	bool mirror_h;

private:
	CResourceRefCounter<CTexture> Texture;
	unsigned long EllapsedTime;
	unsigned long LastTime;
	unsigned int CurrentFrame;
	unsigned int CurrentRow;
	unsigned int CurrentColumn;
	bool isFirstTimeRendering;
	size_t AnimationsCount;
	SAnimationInfo *animations;
	SAnimationInfo *anim;
};

/**
*	New particle system	
*	note:
*		> different interpolation schemes from start point to endpoint
*		> attractors
*		> spawning from particular geometry
*/
class CNewSuperParticleSystem : public CRenderableComponent
{
public:
	int					MaxParticles;				
	float				Life;						//	-1 stands for immortal ps
	int					Emission;					//	Number of particles per frame or second

	float				ParticleLife;
	float				ParticleLifeVariability;

	RGBAf				ColorStart;
	RGBAf				ColorOver;
	RGBAf				ColorVariability;
	
	float				SizeStart;
	float				SizeOver;
	float				SizeVariability;

	int					NotCreated;
	int					ParticlesActive;
	float				Age;

	CNewSuperParticleSystem(int AMaxParticles = 100, int AEmission = 100, float ASystemLife = -1, 
		float AParticleLife = 1, const RGBAf &AColorStart = COLOR_WHITE, const RGBAf &AColorEnd = COLOR_WHITE,
		float ASizeStart = 1, float ASizeEnd = 1, int AParticleLifeVariability = 1, 
		const RGBAf &AColorVariability = COLOR_BLACK, float ASizeVariability = 1);
	~CNewSuperParticleSystem();
	void Update(float dt);
	void CreateParticle();

private:
	struct CNewSuperParticle
	{
		Vector2	PreviousPosition;
		Vector2	Velocity;
		RGBAf DeltaColor;
		float DeltaSize;
		float Age;
	};

	vector<CNewSuperParticle> Particles;
};

/**
*	CMouseCursor	
*	@todo: <strike>make CMouseCusor a separate class</strike>, draw at topmost layer,
*		add possibility to load textured cursors, etc.
*	other @todo: inherit from CRenderComponent and then add to global root object
*		make update position throug events. Compllicity for nothing, as i call it.
*/

class CMouseCursor : public CRenderableComponent
{
public:
	CBox Box;
	CMouseCursor()
	{		
		CPlaceableComponent *tempPlaceable = CFactory::Instance()->New<CPlaceableComponent>();
		tempPlaceable->SetLayer(512);
		SetColor(COLOR_GREEN);
		SetModel(CRenderManager::CreateModelBox(4.0f, 4.0f, MODEL_TYPE_LINES));

		SetParent(tempPlaceable);
		CUpdateManager::Instance()->RootGameObject->Attach(tempPlaceable);
	}

};

#endif // _2DE_GRAPHICS_HIGH_H_
