#include "2de_GraphicsHigh.h"

// Texture coordinates generations for arbitrary polygon
// 	glBegin(GL_TRIANGLE_FAN);
// 	for (unsigned i = 0; i < poly.GetVertexCount(); i++)
// 	{
// #ifdef G_POLY_TEXTURE_ENABLE
// 		glTexCoord2f(poly[i].x/G_POLY_TEX_CELL_SIZE, poly[i].y/G_POLY_TEX_CELL_SIZE);
// #endif 
// 		glVertex2fv(&(poly[i].x));
// 	}
// 	glEnd();

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// New particle system

	void CNewSuperParticleSystem::CreateParticle()
	{
		if (ParticlesActive >= MaxParticles || ParticlesActive >= MaxParticles)
			return;
		int i = ParticlesActive;

		CRenderableComponent *NewRenderable = new CRenderableComponent(CRenderManager::CreateModelCircle(10.0f));
		Attach(NewRenderable);
		ParticlesActive++;

		Particles[i].Age = Random_Float(ParticleLife - ParticleLifeVariability, ParticleLife + ParticleLifeVariability);
	//	NewRenderable->SetPosition(this->GetPosition());
		Particles[i].Velocity = Vector2((rand()%1000 - rand()%1000) / 10.0f, (rand()%1000 - rand()%1000) / 10.0f);
	//	NewRenderable->SetScaling(Random_Int(SizeStart - SizeVariability, SizeStart + SizeVariability));
	//	Particles[i].DeltaSize = (SizeOver - NewRenderable->GetScaling()) / Particles[i].Age;
		NewRenderable->GetColor() = RandomRange(ColorStart - ColorVariability, ColorStart + ColorVariability);
		RGBAf RealEndColor = RandomRange(ColorOver - ColorVariability, ColorOver + ColorVariability);
		Particles[i].DeltaColor = (RealEndColor - NewRenderable->GetColor()) / Particles[i].Age;
	}

	void CNewSuperParticleSystem::Update(float dt)
	{
		for (int i = 0; i < ParticlesActive; i++)
		{
			Particles[i].Age += dt;
			if (Equal(Particles[i].Age, 0.0f))
			{
				//_swap(i, ParticlesActive-1);
				Detach(i);
				ParticlesActive--;
				i--;
				continue;
			}
			CRenderableComponent *TempRenderable = dynamic_cast<CRenderableComponent *>(this->GetChild(i));
			if (TempRenderable == NULL)
				continue;
	//		TempRenderable->SetPosition(TempRenderable->GetPosition() +  Particles[i].Velocity * dt);
			TempRenderable->GetColor() += Particles[i].DeltaColor * dt;
	//		TempRenderable->SetScaling(TempRenderable->GetScaling() + Particles[i].DeltaSize*dt);
		}

		float np = Emission * dt;	// + uncreated;

		if (!Equal(Age, -1.0f))
		{
			Age -= dt;
			if (Age <= 0.0f)
				Emission = 0;
			if (Age <= 0.0f && ParticlesActive == 0)
			{
				Factory->Destroy(this);
				return;
			}
		}

		int t = ParticlesActive;
		for (int i = t; i < t + np; i++)
		{
			CreateParticle();
		}
	}

	CNewSuperParticleSystem::CNewSuperParticleSystem(int AMaxParticles /*= 100*/, int AEmission /*= 100*/, float ASystemLife /*= -1*/, float AParticleLife /*= 1*/, const RGBAf &AColorStart /*= COLOR_WHITE*/, const RGBAf &AColorEnd /*= COLOR_WHITE*/, float ASizeStart /*= 1*/, float ASizeEnd /*= 1*/, int AParticleLifeVariability /*= 1*/, const RGBAf &AColorVariability /*= COLOR_BLACK*/, float ASizeVariability /*= 1*/) : MaxParticles(AMaxParticles),
	Life(ASystemLife), Emission(AEmission), ParticleLife(AParticleLife), 
	ParticleLifeVariability(AParticleLifeVariability), ColorStart(AColorStart),
	ColorOver(AColorEnd), ColorVariability(AColorVariability), SizeStart(ASizeStart),
	SizeOver(ASizeEnd), SizeVariability(ASizeVariability),
	NotCreated(0), ParticlesActive(0), Age(0.0f)
	{

	}

	CNewSuperParticleSystem::~CNewSuperParticleSystem()
	{

	}

	/*
	how to spawn from geometry:
	if (GeometryVertices != NULL)
	{
	int sr = Random_Int(0, GeometryPointsCount - 2);
	Particles[i].Position = GeometryVertices[sr] + (GeometryVertices[sr + 1] - GeometryVertices[sr])*Random_Float(0.0f, 1.0f);
	Particles[i].Velocity = ((GeometryVertices[sr]-GeometryVertices[sr + 1]).GetPerpendicular().Normalized())/0.01f;
	}

	*/

}	//	namespace Deku2D
