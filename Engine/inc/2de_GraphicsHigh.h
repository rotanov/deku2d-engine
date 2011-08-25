#ifndef _2DE_GRAPHICS_HIGH_H_
#define _2DE_GRAPHICS_HIGH_H_

#include "2de_Core.h"
#include "2de_Components.h"
#include "2de_GraphicsLow.h"
#include "2de_Math.h"

/**
*	CSprite	was here. Approach to make animated sprite before is incorrect.
*	Animation should be general case.
*	But we should save: mirror_h, mirror_v as flags
*/

/**
*	New particle system	- Unlikely to sprite this one is ok. But not working.
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
		float AParticleLife = 1, const RGBAf &AColorStart = color::WHITE, const RGBAf &AColorEnd = color::WHITE,
		float ASizeStart = 1, float ASizeEnd = 1, int AParticleLifeVariability = 1, 
		const RGBAf &AColorVariability = color::BLACK, float ASizeVariability = 1);
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

#endif // _2DE_GRAPHICS_HIGH_H_
