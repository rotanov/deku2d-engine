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


//////////////////////////////////////////////////////////////////////////
//CSprite
CSprite::~CSprite()
{
	if (animations != NULL)
		delete [] animations;
}


bool CSprite::LoadFromFile(const string &Filename)
{
// 	CFile file;
// 	file.Open(Filename, CFile::OPEN_MODE_READ);
// 	file.Read(&AnimationsCount);
// 	if (animations != NULL)
// 		delete [] animations;
// 	animations = new SAnimationInfo[AnimationsCount];
// 	file.Read(animations, sizeof(SAnimationInfo)*AnimationsCount);
// 	file.Close();
// 	anim = animations;
	return true;
}

bool CSprite::SaveToFile(const string &Filename)
{
// 	CFile file;
// 	file.Open(Filename, CFile::OPEN_MODE_WRITE);
// 	file.WriteByte(&AnimationsCount);
// 	file.Write(animations, AnimationsCount*sizeof(SAnimationInfo));
// 	file.Close();
	return true;
}

bool CSprite::AddAnimation(bool _isAnimated, float _m_fFrameDelay, float _m_fwidth,
						   float _m_fheight, int _m_nNumFrameColumns, int _m_nNumFrameRows,
						   int _m_nTotalFrames, int _m_nFrameWidth, int _m_nFrameHeight,
						   int _m_nOffsetX, int _m_nOffsetY, int _AnimationIndex, bool _isLoop)
{
	AnimationsCount++;
	SAnimationInfo *temp;
	temp = new SAnimationInfo[AnimationsCount];

	for (int i = 0; i < AnimationsCount - 1; i++)
		temp[i] = animations[i];
	{
		temp[AnimationsCount-1].isAnimated			= _isAnimated;
		temp[AnimationsCount-1].m_fFrameDelay			= _m_fFrameDelay;
		temp[AnimationsCount-1].m_fheight				= _m_fheight;
		temp[AnimationsCount-1].m_fwidth				= _m_fwidth;
		temp[AnimationsCount-1].m_nFrameHeight		= _m_nFrameHeight;
		temp[AnimationsCount-1].m_nFrameWidth			= _m_nFrameWidth;
		temp[AnimationsCount-1].m_nNumFrameColumns	= _m_nNumFrameColumns;
		temp[AnimationsCount-1].m_nNumFrameRows		= _m_nNumFrameRows;
		temp[AnimationsCount-1].m_nOffsetX			= _m_nOffsetX;
		temp[AnimationsCount-1].m_nOffsetY			= _m_nOffsetY;
		temp[AnimationsCount-1].m_nTotalFrames		= _m_nTotalFrames;
		temp[AnimationsCount-1].AnimationIndex		= _AnimationIndex;
		temp[AnimationsCount-1].isLoop				= _isLoop;
	}
	delete [] animations;
	animations = temp;
	anim = animations;
	return true;
}

void CSprite::Render()
{
	if (Texture == NULL)
		return;
	if (anim == NULL || AnimationsCount  == 0)
		return;
	Texture->CheckLoad();

	float fSubRange_s, fSubRange_t,
		fFrame_s, fFrame_t,
		fLowerLeft_s, fLowerLeft_t,
		fLowerRight_s, fLowerRight_t,
		fUpperLeft_s, fUpperLeft_t,
		fUpperRight_s, fUpperRight_t,
		fOffset_s, fOffset_t,
		fRelativeX, fRelativeY;

	//if (!isLoop && m_nFrameNumber < anim->m_nTotalFrames)
	{
		if (isFirstTimeRendering == true)
		{
			isFirstTimeRendering = false;
			LastTime = SDL_GetTicks();
		}
		else
		{
			EllapsedTime = EllapsedTime + SDL_GetTicks() - LastTime;
			LastTime = SDL_GetTicks();
		}
	}

	fSubRange_s = (1.0f / Texture->Width) * (anim->m_nFrameWidth * anim->m_nNumFrameColumns);
	fSubRange_t = (1.0f / Texture->Height) * (anim->m_nFrameHeight * anim->m_nNumFrameRows);
	fFrame_s = fSubRange_s/anim->m_nNumFrameColumns;
	fFrame_t = fSubRange_t/anim->m_nNumFrameRows;

	fLowerLeft_s	= CurrentColumn*fFrame_s;
	fLowerLeft_t	= 1.0f - (CurrentRow*fFrame_t) - fFrame_t;
	fLowerRight_s	= (CurrentColumn*fFrame_s) + fFrame_s;
	fLowerRight_t	= 1.0f - (CurrentRow*fFrame_t) - fFrame_t;
	fUpperRight_s	= (CurrentColumn*fFrame_s) + fFrame_s;
	fUpperRight_t	= 1.0f - (CurrentRow*fFrame_t);
	fUpperLeft_s	= CurrentColumn*fFrame_s;
	fUpperLeft_t	= 1.0f - (CurrentRow*fFrame_t);

	if (anim->m_nOffsetX>0 || anim->m_nOffsetY > 0)
	{
		fOffset_s		= (1.0f / Texture->Width) * anim->m_nOffsetX;
		fOffset_t		= (1.0f / Texture->Height) * anim->m_nOffsetY;
		fLowerLeft_s	= fLowerLeft_s + fOffset_s;
		fLowerLeft_t	= fLowerLeft_t - fOffset_t;
		fLowerRight_s	= fLowerRight_s + fOffset_s;
		fLowerRight_t	= fLowerRight_t - fOffset_t;
		fUpperRight_s	= fUpperRight_s + fOffset_s;
		fUpperRight_t	= fUpperRight_t - fOffset_t;
		fUpperLeft_s	= fUpperLeft_s + fOffset_s;
		fUpperLeft_t	= fUpperLeft_t - fOffset_t;
	}

	fRelativeX = (anim->m_fwidth);//  * 0.5);
	fRelativeY = (anim->m_fheight);// * 0.5);


	//Texture//
	
	
	Vector2 LowerLeft = Vector2((int)fRelativeX * -0.5f, (int)fRelativeY * -0.5f);
	Vector2 UpperRight = Vector2((int)fRelativeX * 0.5f, (int)fRelativeY * 0.5f);
	
// @todo: fix sprite render later
 	if (!mirror_h)
		;
// 		CRenderManager::Instance()->DrawTexturedBox(this,
// 			CBox(LowerLeft, UpperRight), Texture,
// 			CBox(Vector2(fLowerLeft_s, fLowerLeft_t), Vector2(fUpperRight_s, fUpperRight_t)).GetVertices());
 	else
	{
		Vector2Array<4> TexCoords;
		TexCoords[0] = Vector2(fLowerRight_s, fLowerRight_t);
		TexCoords[1] = Vector2(fLowerLeft_s, fLowerLeft_t);
		TexCoords[2] = Vector2(fUpperLeft_s, fUpperLeft_t);
		TexCoords[3] = Vector2(fUpperRight_s, fUpperRight_t);
// 			CRenderManager::Instance()->DrawTexturedBox(this,
// 				CBox(LowerLeft, UpperRight), Texture,
// 				TexCoords);
	}

	if (EllapsedTime >= anim->m_fFrameDelay)
	{
		EllapsedTime = 0;
		CurrentColumn++;
		CurrentFrame++;

		if (CurrentFrame >= anim->m_nTotalFrames)
			if (!anim->isLoop)
			{
				CurrentColumn = anim->m_nNumFrameColumns-1;
				CurrentRow = anim->m_nNumFrameRows-1;
				CurrentFrame = anim->m_nTotalFrames-1;
				return;
			}

			if (CurrentColumn >= anim->m_nNumFrameColumns)
			{
				CurrentColumn = 0;
				CurrentRow++;
			}

			if (CurrentRow >= anim->m_nNumFrameRows)
			{
				CurrentRow		= 0;
				CurrentColumn	= 0;
			}

			if (CurrentFrame >= anim->m_nTotalFrames)
			{
				CurrentRow		= 0;
				CurrentColumn	= 0;
				CurrentFrame		= 0;
			}
	}
}

void CSprite::SetAnimation(int index)
{
	for (int i = 0; i < AnimationsCount; i++)
		if (index == animations[i].AnimationIndex)
		{	
			if (anim != &(animations[i]))
			{
				anim = &(animations[i]);
				CurrentRow		= 0;
				CurrentColumn	= 0;
				CurrentFrame		= 0;
			}
			break;
		}
}

SAnimationInfo* CSprite::FindAnimation(int index)
{
	for (int i = 0; i < AnimationsCount; i++)
		if (index == animations[i].AnimationIndex)		
			return &(animations[i]);
	return NULL;
}

bool CSprite::SetTexture(const string &TextureName)
{
	Texture = CFactory::Instance()->Get<CTexture>(TextureName);
	return true;
}

CSprite::CSprite() : EllapsedTime(0), LastTime(0), CurrentFrame(0), CurrentRow(0), CurrentColumn(0),
isFirstTimeRendering(true), AnimationsCount(0), animations(NULL), anim(NULL),
Color(COLOR_WHITE), mirror_h(false)
{
	SetName("CSprite");
}
//////////////////////////////////////////////////////////////////////////
// Super mega uber new particle system, which better to be named CSpawner

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
			Detach(Children[i]);
			ParticlesActive--;
			i--;
			continue;
		}
		CRenderableComponent *TempRenderable = dynamic_cast<CRenderableComponent *>(Children[i]);
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
			CFactory::Instance()->Destroy(this);
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
