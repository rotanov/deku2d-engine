#include "2de_GraphicsHigh.h"
#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
//PrimitiveRender

int CPrimitiveRender::glListCircleL = 0;
int CPrimitiveRender::glListQuarterCircle =0;
int CPrimitiveRender::glListCircleS = 0;
int CPrimitiveRender::glListRingS = 0;
int CPrimitiveRender::glListHalfCircle = 0;


void CPrimitiveRender::grLine(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	plClr->glSet();
	glBegin(GL_LINES);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grSegment(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	plClr->glSet();
	glBegin(GL_LINES);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	if (ppClr)
		ppClr->glSet();
	glBegin(GL_POINTS);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grSegmentC( const Vector2 &v0, const Vector2 &v1 )
{
	BeforeRndr();
	plClr->glSet();
	glTranslatef(v0.x, v0.y, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	glTranslatef(-v0.x, -v0.y, 0.0f);
	glBegin(GL_LINES);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	if (ppClr)
		ppClr->glSet();
	glBegin(GL_POINTS);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grCircleL(const Vector2 &p, float Radius)
{
	if (!glIsList(glListCircleL))
		return;
	BeforeRndr();
	plClr->glSet();
	glTranslatef(p.x, p.y, depth);
	glScalef(Radius, Radius, 1.0f);
	glCallList(glListCircleL);
	AfterRndr();
}

void CPrimitiveRender::grCircleS(const Vector2 &p, float Radius)
{
	if (!glIsList(glListCircleL))
		return;
	BeforeRndr();
	psClr->glSet();
	glTranslatef(p.x, p.y, depth);
	glScalef(Radius, Radius, 1.0f);
	glCallList(glListCircleS);
	AfterRndr();
}

void CPrimitiveRender::grCircleC(const Vector2 &p, float Radius)
{
	grCircleS(p, Radius);
	grCircleL(p, Radius);
	grSegmentC(p, p + Vector2(Radius, 0.0f));
}

void CPrimitiveRender::grRectL(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	plClr->glSet();
	glBegin(GL_LINE_LOOP);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v0.x, v1.y);
	glEnd();
	AfterRndr();
}
void CPrimitiveRender::grRectS(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	psClr->glSet();
	glBegin(GL_QUADS);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v0.x, v1.y);
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grRectC(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	float hy = (v1.y+v0.y)*0.5f;
	float hx = (v1.x+v0.x)*0.5f;
	glTranslatef(hx, hy, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	glTranslatef(-hx, -hy, 0.0f);
	psClr->glSet();
	glBegin(GL_QUADS);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v0.x, v1.y);
	glEnd();
	plClr->glSet();
	glBegin(GL_LINE_LOOP);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v0.x, v1.y);
	glEnd();
	ppClr->glSet();
	glBegin(GL_POINTS);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v0.x, v1.y);
	glEnd();
	AfterRndr();
}


void CPrimitiveRender::grArrowL(const Vector2& v0, const Vector2& v1)
{
	BeforeRndr();
	float fAngle = atan2(v1.y-v0.y, v1.x-v0.x);
	glTranslatef(v0.x, v0.y, 0.0f);
	glRotatef(RadToDeg(fAngle), 0.0f, 0.0f, 1.0f);
	glScalef((v1-v0).Length(), (v1-v0).Length(), 1.0f);

	plClr->glSet();
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.75f, 0.2f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.75f,-0.2f);
	glEnd();
	ppClr->glSet();

	glBegin(GL_POINTS);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.75f, 0.2f);
	glVertex2f(0.75f, -0.2f);
	glEnd();

	AfterRndr();
}

void CPrimitiveRender::grArrowC(const Vector2 &v0,const Vector2 &v1)
{
	// TODO: complete function
	//grCircleS()
	grRingS(v0, 20);
	grCircleL(v0, 20);
	grCircleL(v0, 20*0.7f); // MAGIC NUMBERS ARRR!
	grArrowL(v0, v1);
}

void CPrimitiveRender::grPolyC(const Vector2 &p, float angle, CPolygon *poly)
{
	BeforeRndr();
	glTranslatef(p.x, p.y, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, -1.0f);
	psClr->glSet();

#ifdef G_POLY_TEXTURE_ENABLE
	glEnable(GL_TEXTURE_2D);
	CTextureManager *Tman = CTextureManager::Instance();
	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObject("cells"));
	cells->Bind();
#endif 

	glBegin(GL_TRIANGLE_FAN);
	for(int i = 0; i < poly->numV; i++)
	{
#ifdef G_POLY_TEXTURE_ENABLE
		glTexCoord2f(poly->V[i].x/G_POLY_TEX_CELL_SIZE, poly->V[i].y/G_POLY_TEX_CELL_SIZE);
#endif 
		glVertex2fv(&(poly->V[i].x));
	}
	glEnd();

#ifdef G_POLY_OUTLINE_ENABLE

	glEnable(GL_LINE_WIDTH);
	glLineWidth(1.0f);
	plClr->glSet();
	glBegin(GL_LINE_LOOP);
	for(int i = 0; i < poly->numV; i++)
	{
		glVertex2fv(&(poly->V[i].x));
	}
	glEnd();

#endif 
	AfterRndr();
}

void CPrimitiveRender::grRingS(const Vector2 &p, float Radius)
{
	BeforeRndr();
	glTranslatef(p.x, p.y, 0.0f);
	psClr->glSet();
	glScalef(Radius, Radius, 0.0f);
	glCallList(glListRingS);
	AfterRndr();
}

void CPrimitiveRender::grRingC( const Vector2 &p, float Radius )
{
	BeforeRndr();
	glTranslatef(p.x, p.y, 0.0f);
	glScalef(Radius, Radius, 0.0f);
	psClr->glSet();
	glCallList(glListRingS);
	glScalef(1.0f/Radius, 1.0f/Radius, 0.0f);
	glTranslatef(-p.x, -p.y, 0.0f);
	grCircleL(p, Radius);
	grCircleL(p, Radius*0.7f); // MAGIC NUMBER!
	AfterRndr();
}

void CPrimitiveRender::BeforeRndr()
{
	glPushMatrix();
	glPushAttrib(GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT | GL_BLEND | GL_LINE_BIT | GL_POINT_BIT);
	glEnable(GL_POINT_SIZE);
	glLineWidth(lwidth);
	glPointSize(psize);
	glEnable(GL_LINE_WIDTH);
	glDisable(GL_TEXTURE_2D);
	if (LineStippleEnabled)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(lwidth*4, 0x0F); // TODO Magic number
	}
	else
		glDisable(GL_LINE_STIPPLE);
	CheckBlend();
	if (doUseGlobalCoordSystem)
		glLoadIdentity();
}

void CPrimitiveRender::AfterRndr()
{
	glPopAttrib();
	glPopMatrix();
}

void CPrimitiveRender::CheckBlend()
{
	switch(BlendingOption)
	{
	case PRM_RNDR_OPT_BLEND_ONE:
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);		
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		break;
	case PRM_RNDR_OPT_BLEND_OTHER:
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case PRM_RNDR_OPT_BLEND_NO_ONE:
		glDisable(GL_BLEND);
		break;
	}
}

void CPrimitiveRender::CheckTexture()
{
	glEnable(GL_TEXTURE_2D);
	CTextureManager *Tman = CTextureManager::Instance();
	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObject("cells"));
	cells->Bind();
}

void CPrimitiveRender::Init()  // TODO: вынести качество генерации хотя бы в константу.
{
	if (!glIsList(glListCircleL))
	{
		glListCircleL = glGenLists(1);
		glNewList(glListCircleL, GL_COMPILE);
		glBegin(GL_LINE_LOOP);
		for(int i = 0; i < 64 + 1; i ++)
		{
			Vector2 P(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
			glVertex2f(P.x, P.y);
		}
		glEnd();
		glEndList();
	}

	if (!glIsList(glListQuarterCircle))
	{
		glListQuarterCircle = glGenLists(1);
		glNewList(glListQuarterCircle, GL_COMPILE);
		glBegin(GL_LINES);
		for(int i = 0; i < 16; i ++)
		{
			Vector2 P(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
			glVertex2f(P.x, P.y);
			P = Vector2(cos(PI * ((i+1) / 32.0f)), sin(PI * ((i+1) / 32.0f)));
			glVertex2f(P.x, P.y);
		}
		glEnd();
		glEndList();
	}

	if (!glIsList(glListCircleS))
	{
		glListCircleS = glGenLists(1);
		glNewList(glListCircleS, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN);
		for(int i = 0; i < 64 + 1; i ++)
		{
			Vector2 P(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
			glVertex2f(P.x, P.y);
		}
		glEnd();
		glEndList();
	}

	if (!glIsList(glListRingS))
	{
		glListRingS = glGenLists(1);
		glNewList(glListRingS, GL_COMPILE);
		glBegin(GL_TRIANGLE_STRIP);
		for(int i = 0; i < 64 + 1; i ++)
		{
			Vector2 P(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
			glVertex2f(P.x, P.y);
			P = Vector2(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
			glVertex2f(P.x*0.7f, P.y*0.7f);
		}
		glEnd();
		glEndList();
	}

	if (!glIsList(glListHalfCircle))
	{
		glListHalfCircle = glGenLists(1);
		glNewList(glListHalfCircle, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN);
		for(int i = 0; i < 32 + 1; i ++)
		{
			Vector2 P(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
			glVertex2f(P.x, P.y);
		}
		glEnd();
		glEndList();
	}


}

void CPrimitiveRender::grInYan(const Vector2 &p, float Radius)
{
	BeforeRndr();
	glTranslatef(p.x, p.y, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	sClr = lClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	psClr->glSet();
	glScalef(Radius, Radius, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.01f);
	glCallList(glListHalfCircle);
	glRotatef(180.0f, 0.0f, 0.0f, -1.0f);
	sClr = lClr = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	psClr->glSet();
	glCallList(glListHalfCircle);
	AfterRndr();

	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	sClr = lClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	grCircleS(Vector2(p.x - Radius*0.5f, p.y), Radius*0.5f);
	sClr = lClr = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	grCircleS(Vector2(p.x - Radius*0.5f, p.y), Radius*0.125f);

	sClr = lClr = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	grCircleS(Vector2(p.x + Radius*0.5f, p.y), Radius*0.5f);
	sClr = lClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	grCircleS(Vector2(p.x + Radius*0.5f, p.y), Radius*0.125f);

	//lClr = RGBAf(0.3f, 0.4f, 0.6f, 0.9f);
	lClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	depth = 0.001f;
	grCircleL(p, Radius/*+lwidth/2.0f*/);
}

void CPrimitiveRender::gDrawBBox( CAABB box )
{
	BeforeRndr();
	float x0 = box.vMin.x, x1 = box.vMax.x, y0 = box.vMin.y, y1 = box.vMax.y; 
	grSegmentC(Vector2(x0, y0), Vector2(x1, y0));
	grSegmentC(Vector2(x1, y0), Vector2(x1, y1));
	grSegmentC(Vector2(x1, y1), Vector2(x0, y1));
	grSegmentC(Vector2(x0, y1), Vector2(x0, y0));
	AfterRndr();
}

void CPrimitiveRender::grQuarterCircle(const Vector2 &v0, float Radius)
{
	if (!glIsList(glListQuarterCircle))
		return;
	BeforeRndr();
	plClr->glSet();
	glTranslatef(v0.x, v0.y, depth);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	glScalef(Radius, Radius, 1.0f);
	glCallList(glListQuarterCircle);
	AfterRndr();
}

//////////////////////////////////////////////////////////////////////////
//CSprite


CSprite::~CSprite()
{
	if (animations != NULL)
		delete [] animations;
}


bool CSprite::LoadFromFile(char* Filename)
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

bool CSprite::SaveToFile(char *Filename)
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

	for(int i=0;i<AnimationsCount-1;i++)
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

	//if(!isLoop && m_nFrameNumber < anim->m_nTotalFrames)
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

	fSubRange_s = (1.0f / Texture->width) * (anim->m_nFrameWidth * anim->m_nNumFrameColumns);
	fSubRange_t = (1.0f / Texture->height) * (anim->m_nFrameHeight * anim->m_nNumFrameRows);
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

	if(anim->m_nOffsetX>0 || anim->m_nOffsetY > 0)
	{
		fOffset_s		= (1.0f / Texture->width ) * anim->m_nOffsetX;
		fOffset_t		= (1.0f / Texture->height) * anim->m_nOffsetY;
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


	//glPushAttrib(GL_TEXTURE_BIT || GL_DEPTH_TEST || GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_LIGHTING);
	Texture->Bind();
	
	
	glBegin( GL_QUADS );
	Color.glSet();
	if (!mirror_h)
	{
		glTexCoord2f(fLowerLeft_s, fLowerLeft_t);
			glVertex2i((int)fRelativeX * -0.5f, (int)fRelativeY * -0.5f);
		glTexCoord2f(fLowerRight_s, fLowerRight_t);
			glVertex2i((int)fRelativeX * 0.5f, (int)fRelativeY * -0.5f);
		glTexCoord2f(fUpperRight_s, fUpperRight_t);
			glVertex2i((int)fRelativeX * 0.5f, (int)fRelativeY * 0.5f);
		glTexCoord2f(fUpperLeft_s, fUpperLeft_t);
			glVertex2i((int)fRelativeX * -0.5f, (int)fRelativeY * 0.5f);
	}
	else
	{
		glTexCoord2f(fLowerRight_s, fLowerRight_t);
			glVertex2i((int)fRelativeX * -0.5f, (int)fRelativeY * -0.5f);
		glTexCoord2f(fLowerLeft_s, fLowerLeft_t);
			glVertex2i((int)fRelativeX * 0.5f, (int)fRelativeY * -0.5f);
		glTexCoord2f(fUpperLeft_s, fUpperLeft_t);
			glVertex2i((int)fRelativeX * 0.5f, (int)fRelativeY * 0.5f);
		glTexCoord2f(fUpperRight_s, fUpperRight_t);
			glVertex2i((int)fRelativeX * -0.5f, (int)fRelativeY * 0.5f);
	}
	glColor4ub(255, 255, 255, 255);
	glEnd();

	glPopAttrib();


	if(EllapsedTime >= anim->m_fFrameDelay)
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

			if(CurrentColumn >= anim->m_nNumFrameColumns)
			{
				CurrentColumn = 0;
				CurrentRow++;
			}

			if(CurrentRow >= anim->m_nNumFrameRows)
			{
				CurrentRow		= 0;
				CurrentColumn	= 0;
			}

			if(CurrentFrame >= anim->m_nTotalFrames)
			{
				CurrentRow		= 0;
				CurrentColumn	= 0;
				CurrentFrame		= 0;
			}
	}
}

void CSprite::SetAnimation(int index)
{
	for(int i=0;i<AnimationsCount;i++)
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
	for(int i=0;i<AnimationsCount;i++)
		if (index == animations[i].AnimationIndex)		
			return &(animations[i]);
	return NULL;
}

bool CSprite::SetTexture(const string &TextureName)
{
	Texture = CFactory::Instance()->Get<CTexture>(TextureName);
	return true;
}
//////////////////////////////////////////////////////////////////////////
//CPartcleSystem

void CParticleSystem::_swap(int i, int j)
{
	CParticle t		= Particles[i];
	Particles[i]	= Particles[j];
	Particles[j]	= t;
}


void CParticleSystem::Init()
{
	this->ParticlesActive = 0;
	this->MaxParticles = 10000;
	this->Particles = new CParticle [MaxParticles];
	Emission = 500;
	Age = 0;
	SizeVariability = 1;
	ParticleLifeVariability = 1;
	Life = -1;
	ParticleLife = 1;

	user_create = false;
	user_update = false;

	Texture = NULL;

	ColorStart = RGBAf(1.0,0.0,0.0,0.5);
	ColorOver = RGBAf(0.0,0.0,1.0,0.5);
	ColorVariability = RGBAf(0.0f,0.0f,0.0f,0.0f);
	GeometryPointsCount = 0;
	GeometryVertices  = NULL;
}

void CParticleSystem::Update(float dt)
{
	if (PtrPosition != NULL)
	{
		Position = *PtrPosition;
		//CRenderObject::Position = *PtrPosition;
	}
#ifdef _DEBUG_DISABLE_PARTICLES_UPDATE
	return false;
#endif
	// Here integrating and updating values of active particles
	for(int i=0; i < ParticlesActive; i++)
	{
		if (!user_update)
		{
			Particles[i].Age += dt;
			if (Particles[i].Age >= Particles[i].Life
				)
			{
				_swap(i, ParticlesActive-1);
				ParticlesActive--;
				i--;
				continue;
			}
			Particles[i].Position += Particles[i].Velocity*dt;
			Particles[i].Color += (Particles[i].DeltaColor*dt);
			Particles[i].Size += Particles[i].DeltaSize*dt;

		}
		else
		{
			procUserUpdate(&(Particles[i]), dt);
		}
	}


	float np = Emission * dt;	// + uncreated;

	if (Life != -1)
	{
		Age += dt;
		if (Age >= Life)
			Emission = 0;
		if (Age >= Life && ParticlesActive == 0)
		{
			Dead = true;
			return;
		}
	}

	int t = ParticlesActive;
	for (int i = t; i < t + np; i++)
	{
		CreateParticle();
		// 		if (i >= info.MaxParticles)
		// 			break;
		// 		if (info.ParticlesActive >= info.MaxParticles)
		// 			break;
		// 		info.ParticlesActive++;
		// 
		// 		particles[i].life  = Random_Float(info.plife ,info.plife + info.plifevar); // - rand()%(int)info.plifevar
		// 		if (particles[i].life < 0 ) particles[i].life = info.plife;
		// 		particles[i].age = 0;
		// 		
		// 		if (info.GeomNumPoints == 1)
		// 		{
		// 			particles[i].p = info.geom[0];
		// 			particles[i].v = Vector2( (rand()%1000 - rand()%1000) / 1.0f, (rand()%1000 - rand()%1000) / 1.0f); 
		// 		}
		// 		else
		// 			if (info.geom != NULL)
		// 			{
		// 				int sr = Random_Int(0, info.GeomNumPoints-2);
		// 				particles[i].p = info.geom[sr] + (info.geom[sr+1]-info.geom[sr])*Random_Float(0.0f, 1.0f);
		// 				particles[i].v = ((info.geom[sr]-info.geom[sr+1]).GetPerp().Normalized())/0.01f; 
		// 			}
		// 			else
		// 			{
		// 				particles[i].p = info.p;
		// 				particles[i].v = Vector2( (rand()%1000 - rand()%1000) / 1.0f, (rand()%1000 - rand()%1000) / 1.0f); 
		// 			}
		// 		particles[i].Period = Random_Float(0.0f, 2*PI);
		// 
		// 		particles[i].size = info.startsize + (rand()&(int)info.sizevar - rand()&(int)info.sizevar);
		// 		particles[i].dsize = (info.endsize - info.endsize) / particles[i].life;
		// 
		// 		 
		// 		particles[i].c.a = Random_Float(info.sc.a ,info.sc.a + info.vc.a);
		// 		particles[i].c.r = Random_Float(info.sc.r ,info.sc.r + info.vc.r);
		// 		particles[i].c.g = Random_Float(info.sc.g ,info.sc.g + info.vc.g);
		// 		particles[i].c.b = Random_Float(info.sc.b ,info.sc.b + info.vc.b);
		// 
		// 		particles[i].dc = (info.ec - particles[i].c)/particles[i].life;
	}
}

void CParticleSystem::Render()
{
#ifdef _DEBUG_DISABLE_PARTICLES_DRAW
	return false;
#endif
	if (!Texture)
	{
		glPushAttrib(GL_TEXTURE_BIT | GL_POINTS);
		glEnable(GL_POINTS);
		glDisable(GL_TEXTURE_2D);
		glPointSize(Particles[0].Size);
		glBegin(GL_POINTS);
		for(int i = 0; i < ParticlesActive; i++)
		{
			Particles[i].Color.glSet();
			glVertex2f(Particles[i].Position.x, Particles[i].Position.y);			
		}
		glEnd();
		glPopAttrib();
	}
	else
	{
		glPushAttrib(GL_TEXTURE_BIT | GL_POINTS);
		glDisable(GL_POINTS);
		glEnable(GL_TEXTURE_2D);


		//glBlendFunc(GL_SRC_ALPHA,GL_ONE);


		Texture->Bind();

		glBegin(GL_QUADS);
		for(int i = 0; i < ParticlesActive; i++)
		{
			Particles[i].Color.glSet();
			glTexCoord2f(0.0f, 0.0f);
				glVertex2f(Particles[i].Position.x - Particles[i].Size,	Particles[i].Position.y - Particles[i].Size);			
			glTexCoord2f(1.0f, 0.0f);
				glVertex2f(Particles[i].Position.x + Particles[i].Size,	Particles[i].Position.y - Particles[i].Size);			
			glTexCoord2f(1.0f, 1.0f);
				glVertex2f(Particles[i].Position.x + Particles[i].Size,	Particles[i].Position.y + Particles[i].Size);			
			glTexCoord2f(0.0f, 1.0f);
				glVertex2f(Particles[i].Position.x - Particles[i].Size,	Particles[i].Position.y + Particles[i].Size);			
		}
		glEnd();
		glPopAttrib();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

bool CParticleSystem::SaveToFile()
{
	CFile file;
	file.Open(Filename, CFile::OPEN_MODE_WRITE);
	//file.Write(&Info, sizeof(CPsysteminfo));
	file.Close();
	return true;
}

bool CParticleSystem::LoadFromFile()
{
	CFile file;
	file.Open(Filename, CFile::OPEN_MODE_READ);
	//file.Read(&Info, sizeof(CPsysteminfo));
	file.Close();
	return true;
}

void CParticleSystem::SetGeometry( Vector2 * points, int numPoints )
{
	GeometryVertices = points;
	GeometryPointsCount = numPoints;
}

CParticle* CParticleSystem::CreateParticle()
{
	int t = ParticlesActive;
	int i = t;
	{
		if (i >= MaxParticles)
			return NULL;
		if (ParticlesActive >= MaxParticles)
			return NULL;
		ParticlesActive++;

		if (!user_create)
		{
			Particles[i].Life  = Random_Float(ParticleLife, ParticleLife + ParticleLifeVariability);
			if (Particles[i].Life < 0 )
				Particles[i].Life = ParticleLife;
			Particles[i].Age = 0;

			if (GeometryPointsCount == 1)
			{
				Particles[i].Position = GeometryVertices[0];
				Particles[i].Velocity = Vector2( (rand() % 1000 - rand() % 1000) / 1.0f, (rand() % 1000 - rand() % 1000) / 1.0f); // MAGIC NUMBERS ARE BAD BAD BAD BAD BAD
			}
			else
				if (GeometryVertices != NULL)
				{
					int sr = Random_Int(0, GeometryPointsCount - 2);
					Particles[i].Position = GeometryVertices[sr] + (GeometryVertices[sr + 1] - GeometryVertices[sr])*Random_Float(0.0f, 1.0f);
					Particles[i].Velocity = ((GeometryVertices[sr]-GeometryVertices[sr + 1]).GetPerpendicular().Normalized())/0.01f; 
				}
				else
				{
					Particles[i].Position = Position;
					Particles[i].Velocity = Vector2( (rand()%1000 - rand()%1000) / 10.0f, (rand()%1000 - rand()%1000) / 10.0f); 
				}

				Particles[i].Size = Random_Int(SizeStart, SizeStart + SizeVariability);
				Particles[i].DeltaSize = (SizeOver - Particles[i].Size) / Particles[i].Life;

				Particles[i].Color.w = Random_Float(ColorStart.w, ColorStart.w + ColorVariability.w);
				Particles[i].Color.x = Random_Float(ColorStart.x, ColorStart.x + ColorVariability.x);
				Particles[i].Color.y = Random_Float(ColorStart.y, ColorStart.y + ColorVariability.y);
				Particles[i].Color.z = Random_Float(ColorStart.z, ColorStart.z + ColorVariability.z);

				Particles[i].DeltaColor = (ColorOver - Particles[i].Color) / Particles[i].Life;
		}
		else
		{
			procUserCreate(&(Particles[i]));
		}
	}
	return &Particles[i];
}

void CParticleSystem::SetUserUpdate( FUpdateFunc func)
{
	if (func == NULL)
		return;
	user_update = true;
	procUserUpdate = func;
}

void CParticleSystem::SetUserCreate( FCreateFunc func )
{
	if (func == NULL)
		return;
	user_create = true;
	procUserCreate = func;
}

CParticleSystem::CParticleSystem() : PtrPosition(NULL)
{
	Texture = NULL;
	Particles = NULL;
	user_create = user_update = user_render = false;
}


CParticleSystem::~CParticleSystem()
{
	SAFE_DELETE(Particles);
}

CText::CText()
{
	Font = CFactory::Instance()->Get<CFont>("Font");
	Text = "";
}

void CText::Render()
{
	COLOR_WHITE.glSet();
	Font->Print(Text.c_str());
}