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
	glColor4fv(&(plClr->r));
	glBegin(GL_LINES);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grSegment(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	glColor4fv(&(plClr->r));
	glBegin(GL_LINES);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	if (ppClr)
		glColor4fv(&(ppClr->r));
	glBegin(GL_POINTS);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grSegmentC( const Vector2 &v0, const Vector2 &v1 )
{
	BeforeRndr();
	glColor4fv(&(plClr->r));
	glTranslatef(v0.x, v0.y, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	glTranslatef(-v0.x, -v0.y, 0.0f);
	glBegin(GL_LINES);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	if (ppClr)
		glColor4fv(&(ppClr->r));
	glBegin(GL_POINTS);
	glVertex2fv(&(v0.x));
	glVertex2fv(&(v1.x));
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grCircleL(const Vector2 &p, scalar Radius)
{
	if (!glIsList(glListCircleL))
		return;
	BeforeRndr();
	glColor4fv(&(plClr->r));
	glTranslatef(p.x, p.y, depth);
	glScalef(Radius, Radius, 1.0f);
	glCallList(glListCircleL);
	AfterRndr();
}

void CPrimitiveRender::grCircleS(const Vector2 &p, scalar Radius)
{
	if (!glIsList(glListCircleL))
		return;
	BeforeRndr();
	glColor4fv(&(psClr->r));
	glTranslatef(p.x, p.y, depth);
	glScalef(Radius, Radius, 1.0f);
	glCallList(glListCircleS);
	AfterRndr();
}

void CPrimitiveRender::grCircleC(const Vector2 &p, scalar Radius)
{
	grCircleS(p, Radius);
	grCircleL(p, Radius);
	grSegmentC(p, p + Vector2(Radius, 0.0f));
}

void CPrimitiveRender::grRectL(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	glColor4fv(&(plClr->r));
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
	glColor4fv(&(psClr->r));
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
	glColor4fv(&(psClr->r));
	glBegin(GL_QUADS);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v0.x, v1.y);
	glEnd();
	glColor4fv(&(plClr->r));
	glBegin(GL_LINE_LOOP);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v0.x, v1.y);
	glEnd();
	glColor4fv(&(ppClr->r));
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

	glColor4fv(&(plClr->r));
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.75f, 0.2f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.75f,-0.2f);
	glEnd();
	glColor4fv(&(ppClr->r));

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

void CPrimitiveRender::grPolyC(const Vector2 &p, scalar angle, CPolygon *poly)
{
	BeforeRndr();
	glTranslatef(p.x, p.y, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, -1.0f);
	glColor4fv(&(psClr->r));

#ifdef G_POLY_TEXTURE_ENABLE
	glEnable(GL_TEXTURE_2D);
	CTextureManager *Tman = CTextureManager::Instance();
	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObject((string*)"cells"));
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
	glColor4fv(&(plClr->r));
	glBegin(GL_LINE_LOOP);
	for(int i = 0; i < poly->numV; i++)
	{
		glVertex2fv(&(poly->V[i].x));
	}
	glEnd();

#endif 
	AfterRndr();
}

void CPrimitiveRender::grRingS(const Vector2 &p, scalar Radius)
{
	BeforeRndr();
	glTranslatef(p.x, p.y, 0.0f);
	glColor4fv(&(psClr->r));
	glScalef(Radius, Radius, 0.0f);
	glCallList(glListRingS);
	AfterRndr();
}

void CPrimitiveRender::grRingC( const Vector2 &p, scalar Radius )
{
	BeforeRndr();
	glTranslatef(p.x, p.y, 0.0f);
	glScalef(Radius, Radius, 0.0f);
	glColor4fv(&(psClr->r));
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
	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObject((string*)"cells"));
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

void CPrimitiveRender::grInYan(const Vector2 &p, scalar Radius)
{
	BeforeRndr();
	glTranslatef(p.x, p.y, 0.0f);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	sClr = lClr = RGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	glColor4fv(&(psClr->r));
	glScalef(Radius, Radius, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.01f);
	glCallList(glListHalfCircle);
	glRotatef(180.0f, 0.0f, 0.0f, -1.0f);
	sClr = lClr = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	glColor4fv(&(psClr->r));
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
	float x0 = box.x0, x1 = box.x1, y0 = box.y0, y1 = box.y1; 
	grSegmentC(Vector2(x0, y0), Vector2(x1, y0));
	grSegmentC(Vector2(x1, y0), Vector2(x1, y1));
	grSegmentC(Vector2(x1, y1), Vector2(x0, y1));
	grSegmentC(Vector2(x0, y1), Vector2(x0, y0));
	AfterRndr();
}

void CPrimitiveRender::grQuarterCircle(const Vector2 &v0, scalar Radius)
{
	if (!glIsList(glListQuarterCircle))
		return;
	BeforeRndr();
	glColor4fv(&(plClr->r));
	glTranslatef(v0.x, v0.y, depth);
	glRotatef(Angle, 0.0f, 0.0f, -1.0f);
	glScalef(Radius, Radius, 1.0f);
	glCallList(glListQuarterCircle);
	AfterRndr();
}

//////////////////////////////////////////////////////////////////////////
//CSprite

CSprite::CSprite()
{
	m_bFirstRendering = true;
	color = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	animations = NULL;
	anim = NULL;
	numAnimations = 0;
	ellapsedtime = 0;
	lasttime = 0;
	m_nFrameNumber = 0;
	m_nCurrentRow = 0;
	m_nCurrentColumn = 0;	
	mirror_h = false;
}

CSprite::~CSprite()
{
	if (animations != NULL)
		delete [] animations;
}

bool CSprite::LoadTexture(char *filename)
{
	CGLImageData temp;
	temp.LoadTexture(filename);
	m_textureID = temp.GetTexID();
	m_nTextureWidth = temp.width;
	m_nTextureHeight = temp.height;	
	return true;
}

bool CSprite::LoadFromFile(char* filename)
{
	CFile file;
	file.Open(filename, CFile::OPEN_MODE_READ);
	file.ReadByte(&numAnimations);
	if (animations != NULL)
		delete [] animations;
	animations = new SAnimationInfo[numAnimations];
	file.Read(animations, sizeof(SAnimationInfo)*numAnimations);
	file.Close();
	anim = animations;
	return true;
}

bool CSprite::SaveToFile(char *filename)
{
	CFile file;
	file.Open(filename, CFile::OPEN_MODE_WRITE);
	file.WriteByte(&numAnimations);
	file.Write(animations, numAnimations*sizeof(SAnimationInfo));
	file.Close();
	return true;
}

bool CSprite::AddAnimation(bool _isAnimated, float _m_fFrameDelay, float _m_fwidth,
						   float _m_fheight, int _m_nNumFrameColumns, int _m_nNumFrameRows,
						   int _m_nTotalFrames, int _m_nFrameWidth, int _m_nFrameHeight,
						   int _m_nOffsetX, int _m_nOffsetY, int _AnimationIndex, bool _isLoop)
{
	numAnimations++;
	SAnimationInfo *temp;	
	temp = new SAnimationInfo[numAnimations];

	for(int i=0;i<numAnimations-1;i++)
		temp[i] = animations[i];
	{
		temp[numAnimations-1].isAnimated			= _isAnimated;
		temp[numAnimations-1].m_fFrameDelay			= _m_fFrameDelay;
		temp[numAnimations-1].m_fheight				= _m_fheight;
		temp[numAnimations-1].m_fwidth				= _m_fwidth;
		temp[numAnimations-1].m_nFrameHeight		= _m_nFrameHeight;
		temp[numAnimations-1].m_nFrameWidth			= _m_nFrameWidth;
		temp[numAnimations-1].m_nNumFrameColumns	= _m_nNumFrameColumns;
		temp[numAnimations-1].m_nNumFrameRows		= _m_nNumFrameRows;
		temp[numAnimations-1].m_nOffsetX			= _m_nOffsetX;
		temp[numAnimations-1].m_nOffsetY			= _m_nOffsetY;
		temp[numAnimations-1].m_nTotalFrames		= _m_nTotalFrames;
		temp[numAnimations-1].AnimationIndex		= _AnimationIndex;
		temp[numAnimations-1].isLoop				= _isLoop;
	}
	delete [] animations;
	animations = temp;
	anim = animations;
	return true;
}

bool CSprite::Render()
{
	if (anim == NULL || numAnimations  == 0)
		return false;

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
		if (m_bFirstRendering == true)
		{
			m_bFirstRendering = false;
			lasttime = SDL_GetTicks();
		}
		else
		{
			ellapsedtime = ellapsedtime + SDL_GetTicks() - lasttime;
			lasttime = SDL_GetTicks();
		}
	}

	fSubRange_s = (1.0f/m_nTextureWidth)*(anim->m_nFrameWidth*anim->m_nNumFrameColumns);
	fSubRange_t = (1.0f/m_nTextureHeight)*(anim->m_nFrameHeight*anim->m_nNumFrameRows);
	fFrame_s = fSubRange_s/anim->m_nNumFrameColumns;
	fFrame_t = fSubRange_t/anim->m_nNumFrameRows;

	fLowerLeft_s	= m_nCurrentColumn*fFrame_s;
	fLowerLeft_t	= 1.0f - (m_nCurrentRow*fFrame_t) - fFrame_t;
	fLowerRight_s	= (m_nCurrentColumn*fFrame_s) + fFrame_s;
	fLowerRight_t	= 1.0f - (m_nCurrentRow*fFrame_t) - fFrame_t;
	fUpperRight_s	= (m_nCurrentColumn*fFrame_s) + fFrame_s;
	fUpperRight_t	= 1.0f - (m_nCurrentRow*fFrame_t);
	fUpperLeft_s	= m_nCurrentColumn*fFrame_s;
	fUpperLeft_t	= 1.0f - (m_nCurrentRow*fFrame_t);

	if(anim->m_nOffsetX>0 || anim->m_nOffsetY > 0)
	{
		fOffset_s		= (1.0f/m_nTextureWidth )*anim->m_nOffsetX;
		fOffset_t		= (1.0f/m_nTextureHeight)*anim->m_nOffsetY;
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


	glPushAttrib(GL_TEXTURE_BIT || GL_DEPTH_TEST || GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glBegin( GL_QUADS );
	color.glSet();
	if (!mirror_h)
	{
		glTexCoord2f(fLowerLeft_s, fLowerLeft_t);
		glVertex2i( 0, 0);
		glTexCoord2f(fLowerRight_s, fLowerRight_t);
		glVertex2i( (int)fRelativeX, 0 );
		glTexCoord2f(fUpperRight_s, fUpperRight_t);
		glVertex2i( (int)fRelativeX, (int)fRelativeY );
		glTexCoord2f(fUpperLeft_s, fUpperLeft_t);
		glVertex2i(0, (int)fRelativeY );
	}
	else
	{
		glTexCoord2f(fLowerRight_s, fLowerRight_t);
		glVertex2i( 0, 0);
		glTexCoord2f(fLowerLeft_s, fLowerLeft_t);
		glVertex2i( (int)fRelativeX, 0 );
		glTexCoord2f(fUpperLeft_s, fUpperLeft_t);
		glVertex2i( (int)fRelativeX, (int)fRelativeY );
		glTexCoord2f(fUpperRight_s, fUpperRight_t);
		glVertex2i(0, (int) fRelativeY );
	}
	glColor4ub(255, 255, 255, 255);
	glEnd();

	glPopAttrib();


	if(ellapsedtime >= anim->m_fFrameDelay)
	{
		ellapsedtime = 0;
		m_nCurrentColumn++;
		m_nFrameNumber++;

		if (m_nFrameNumber >= anim->m_nTotalFrames)
			if (!anim->isLoop)
			{
				m_nCurrentColumn = anim->m_nNumFrameColumns-1;
				m_nCurrentRow = anim->m_nNumFrameRows-1;
				m_nFrameNumber = anim->m_nTotalFrames-1;
				return true;
			}

			if(m_nCurrentColumn >= anim->m_nNumFrameColumns)
			{
				m_nCurrentColumn = 0;
				m_nCurrentRow++;
			}

			if(m_nCurrentRow >= anim->m_nNumFrameRows)
			{
				m_nCurrentRow		= 0;
				m_nCurrentColumn	= 0;
			}

			if(m_nFrameNumber >= anim->m_nTotalFrames)
			{
				m_nCurrentRow		= 0;
				m_nCurrentColumn	= 0;
				m_nFrameNumber		= 0;
			}
	}
	return true;
}

bool CSprite::Render(float x, float y)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	Render();
	glPopMatrix();
	return true;
}

void CSprite::SetAnimation(int index)
{
	for(int i=0;i<numAnimations;i++)
		if (index == animations[i].AnimationIndex)
		{	
			if (anim != &(animations[i]))
			{
				anim = &(animations[i]);
				m_nCurrentRow		= 0;
				m_nCurrentColumn	= 0;
				m_nFrameNumber		= 0;
			}
			break;
		}
}

SAnimationInfo* CSprite::FindAnimation(int index)
{
	for(int i=0;i<numAnimations;i++)
		if (index == animations[i].AnimationIndex)		
			return &(animations[i]);
	return NULL;
}

CSprite* CSprite::Instance()
{
	return this;
}

//////////////////////////////////////////////////////////////////////////
//CPartcleSystem

void CParticleSystem::_swap(int i, int j)
{
	CParticle t		= particles[i];
	particles[i]	= particles[j];
	particles[j]	= t;
}


void CParticleSystem::Init()
{
	memset(&Info, 0, sizeof(Info));

	this->Info.ParticlesActive = 0;
	this->Info.MaxParticles = 10000;
	this->particles = new CParticle [Info.MaxParticles];
	Info.emission = 500;
	Info.age = 0;
	Info.sizevar = 1;
	Info.plifevar = 1;
	Info.life = -1;
	Info.plife = 1;

	user_create = false;
	user_update = false;

	Texture = NULL;

	Info.sc = RGBAf(1.0,0.0,0.0,0.5);
	Info.ec = RGBAf(0.0,0.0,1.0,0.5);
	Info.vc = RGBAf(0.0f,0.0f,0.0f,0.0f);
}

bool CParticleSystem::Update(scalar dt)
{
#ifdef _DEBUG_DISABLE_PARTICLES_UPDATE
	return false;
#endif
	// Here integrating and updating values of active particles
	for(int i=0; i < Info.ParticlesActive; i++)
	{
		if (!user_update)
		{
			particles[i].age += dt;
			if (particles[i].age >= particles[i].life
				)
			{
				_swap(i, Info.ParticlesActive-1);
				Info.ParticlesActive--;
				i--;
				continue;
			}
			particles[i].p += particles[i].v*dt;
			particles[i].c += (particles[i].dc*dt);
			particles[i].size += particles[i].dsize;

		}
		else
		{
			procUserUpdate(&(particles[i]), dt);
		}
	}


	float np = Info.emission * dt;	// + uncreated;

	if (Info.life != -1)
	{
		Info.age += dt;
		if (Info.age >= Info.life)
			return true;
	}

	int t = Info.ParticlesActive;
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
	return true;
}

bool CParticleSystem::Render()
{
#ifdef _DEBUG_DISABLE_PARTICLES_DRAW
	return false;
#endif
	if (!Texture)
	{
		glPushAttrib(GL_TEXTURE_BIT | GL_POINTS);
		glEnable(GL_POINTS);
		glDisable(GL_TEXTURE_2D);
		glPointSize(particles[0].size);
		glBegin(GL_POINTS);
		for(int i=0;i<Info.ParticlesActive;i++)
		{
			particles[i].c.glSet();
			glVertex2f(particles[i].p.x, particles[i].p.y);			
		}
		glEnd();
		glPopAttrib();
	}
	else
	{
		glPushAttrib(GL_TEXTURE_BIT | GL_POINTS);
		glDisable(GL_POINTS);
		glEnable(GL_TEXTURE_2D);


		glBlendFunc(GL_SRC_ALPHA,GL_ONE);


		Texture->Bind();

		glBegin(GL_QUADS);
		for(int i=0;i<Info.ParticlesActive;i++)
		{
			gSetColor(particles[i].c);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(particles[i].p.x,						particles[i].p.y					);			
			glTexCoord2f(1.0f, 0.0f); glVertex2f(particles[i].p.x + particles[i].size,	particles[i].p.y					);			
			glTexCoord2f(1.0f, 1.0f); glVertex2f(particles[i].p.x + particles[i].size,	particles[i].p.y + particles[i].size);			
			glTexCoord2f(0.0f, 1.0f); glVertex2f(particles[i].p.x,						particles[i].p.y + particles[i].size);			
		}
		glEnd();
		glPopAttrib();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	return true;
}

bool CParticleSystem::SaveToFile()
{
	CFile file;
	file.Open(filename, CFile::OPEN_MODE_WRITE);
	file.Write(&Info, sizeof(CPsysteminfo));
	file.Close();
	return true;
}

bool CParticleSystem::LoadFromFile()
{
	CFile file;
	file.Open(filename, CFile::OPEN_MODE_READ);
	file.Read(&Info, sizeof(CPsysteminfo));
	file.Close();
	return true;
}

void CParticleSystem::SetGeometry( Vector2 * points, int numPoints )
{
	Info.geom = points;
	Info.GeomNumPoints = numPoints;
}

CParticle	*CParticleSystem::CreateParticle()
{
	int t = Info.ParticlesActive;
	int i = t;
	{
		if (i >= Info.MaxParticles)
			return NULL;
		if (Info.ParticlesActive >= Info.MaxParticles)
			return NULL;
		Info.ParticlesActive++;

		if (!user_create)
		{
			particles[i].life  = Random_Float(Info.plife ,Info.plife + Info.plifevar);
			if (particles[i].life < 0 )
				particles[i].life = Info.plife;
			particles[i].age = 0;

			if (Info.GeomNumPoints == 1)
			{
				particles[i].p = Info.geom[0];
				particles[i].v = Vector2( (rand()%1000 - rand()%1000) / 1.0f, (rand()%1000 - rand()%1000) / 1.0f); 
			}
			else
				if (Info.geom != NULL)
				{
					int sr = Random_Int(0, Info.GeomNumPoints-2);
					particles[i].p = Info.geom[sr] + (Info.geom[sr+1]-Info.geom[sr])*Random_Float(0.0f, 1.0f);
					particles[i].v = ((Info.geom[sr]-Info.geom[sr+1]).GetPerp().Normalized())/0.01f; 
				}
				else
				{
					particles[i].p = Info.p;
					particles[i].v = Vector2( (rand()%1000 - rand()%1000) / 1.0f, (rand()%1000 - rand()%1000) / 1.0f); 
				}

				particles[i].size = Random_Int(Info.startsize, Info.startsize + Info.sizevar);
				particles[i].dsize = (Info.endsize - Info.endsize) / particles[i].life;

				particles[i].c.a = Random_Float(Info.sc.a ,Info.sc.a + Info.vc.a);
				particles[i].c.r = Random_Float(Info.sc.r ,Info.sc.r + Info.vc.r);
				particles[i].c.g = Random_Float(Info.sc.g ,Info.sc.g + Info.vc.g);
				particles[i].c.b = Random_Float(Info.sc.b ,Info.sc.b + Info.vc.b);

				particles[i].dc = (Info.ec - particles[i].c)/particles[i].life;
		}
		else
		{
			procUserCreate(&(particles[i]));
		}
	}
	return &particles[i];
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

CParticleSystem::CParticleSystem()
{
	Texture = NULL;
	particles = NULL;
	user_create = user_update = user_render = false;
}


CParticleSystem::~CParticleSystem()
{
	SAFE_DELETE(particles);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//GUI
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
///CGUIObject

CGUIObject::CGUIObject()
{
	Font = NULL;
	PRender = NULL;
	Parent = NULL;
	CallProc = NULL;
	WidgetMouseState = wmsOutside;
	CEngine::Instance()->RenderManager.AddObject(this);
	CEngine::Instance()->UpdateManager.AddObject(this);	
	GuiManager.AddObject(this);
}

CGUIObject::~CGUIObject()
{
	CEngine::Instance()->RenderManager.DelObject(id);
	CEngine::Instance()->UpdateManager.DelObject(id);
}

void CGUIObject::SetFont(CFont *AFont)
{
	Font = AFont;
}

void CGUIObject::SetPrimitiveRender(CPrimitiveRender *APrimitiveRender)
{
	PRender = APrimitiveRender;
// 	PRender->plClr = &color;
// 	PRender->psClr = &color;
// 	PRender->ppClr = &color;
}

void CGUIObject::SetParent(CGUIObject *AParent)
{
	if (!AParent)
		return;
	Parent = AParent;
	Font = AParent->Font;
	PRender = AParent->PRender;
	color = AParent->color;
}
//////////////////////////////////////////////////////////////////////////
///CGUIManager

CGUIManager::CGUIManager(): KeyHoldRepeatInterval(50), KeyHoldRepeatDelay(300), tabholded(false), TimerAccum(0)
{
	name = "Mini GUI manager";
	FocusedOn = NULL;
	SetPrimitiveRender(new CPrimitiveRender);
	CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
}

bool CGUIManager::Update( scalar dt )
{
	if (tabholded)
	{
		TimerAccum += dt*1000;
		if (!repeatstarted)
		{
			if (TimerAccum > KeyHoldRepeatDelay)
			{
				TimerAccum = 0;
				repeatstarted = true;
			}
		}
		else
		{
			if (TimerAccum > KeyHoldRepeatInterval)
			{
				TimerAccum = 0;
				InputHandling(KEY_PRESSED, SDLK_TAB, SDLMod(0), 0);
			}
		}
	}
	else
	{
		TimerAccum = 0;
		repeatstarted = false;
	}
	return true;
}

bool CGUIManager::Render()
{
	glLoadIdentity();
	if (FocusedOn)
	{
		PRender->LineStippleEnabled = true;
		PRender->lwidth = 0.5f;
		PRender->grRectL(FocusedOn->aabb.Inflated(5, 5).vMin, FocusedOn->aabb.Inflated(5, 5).vMax);	
		PRender->LineStippleEnabled = false;
		PRender->lwidth = 1.0f;
	}
	//FocusedOn->PRender->gDrawBBox(FocusedOn->aabb.Inflated(5, 5));
	return true;
}

bool CGUIManager::InputHandling( Uint8 state, Uint16 key, SDLMod mod, char letter )
{
	switch(state)
	{
	case KEY_PRESSED:
		switch(key)
		{
		case SDLK_TAB:
			tabholded = true;
			if (FocusedOnListNode == NULL)
			{
				FocusedOnListNode = GetFirst();
				if (GetFirst())
					FocusedOn = dynamic_cast<CGUIObject*>(GetFirst()->GetData());
			}
			else
			{
				FocusedOnListNode = FocusedOnListNode->next;
				if (FocusedOnListNode)
					FocusedOn = dynamic_cast<CGUIObject*>(FocusedOnListNode->GetData());

			}

			if (FocusedOnListNode == NULL)
			{
				FocusedOnListNode = GetFirst();
				if (GetFirst())
					FocusedOn = dynamic_cast<CGUIObject*>(GetFirst()->GetData());
			}
			break;
		default:
			if(FocusedOnListNode)
				FocusedOn->InputHandling(state, key, mod, letter);
			break;
		}
		break;
	case KEY_RELEASED:
		switch(key)
		{
		case SDLK_TAB:
			tabholded =  false;
			break;
		}
		break;
	}
	return true;
}

void CGUIManager::SetFocusedNodeTo(CListNode* AFocusedNode)
{
	FocusedOnListNode = AFocusedNode;
	if (FocusedOnListNode)
		FocusedOn = dynamic_cast<CGUIObject*>(FocusedOnListNode->GetData());
}

void CGUIManager::SetFocus(CObject* AObject)
{
	CListNode* ListNode = GetListNode(AObject);
	if (ListNode)
	{
		FocusedOnListNode = ListNode;
		FocusedOn = dynamic_cast<CGUIObject*>(FocusedOnListNode->GetData());
	}
}

CGUIManager::~CGUIManager()
{
	SAFE_DELETE(PRender);
}
CGUIManager GuiManager;
//////////////////////////////////////////////////////////////////////////
///CButton

CButton::CButton( CAABB ARect, char* AText, RGBAf AColor, Callback AOnClick )
{
	CallProc = AOnClick;
	aabb = ARect;
	color = AColor;
	text = AText;
}

CButton::CButton()
{
	aabb = CAABB(0, 0, 100, 100);
	color = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);
	text = "You dumb! You called default constructor!";
}

bool CButton::Render()
{	
	CEngine *engine = CEngine::Instance();
	Font = engine->FontManager->GetFont("Font");

	Font->Pos = (aabb.vMin + aabb.vMax) / 2.0f - Vector2(Font->GetStringWidth(text.c_str()), Font->GetStringHeight(text.c_str())) / 2.0f;
	Font->tClr = color;
	glLoadIdentity();
	PRender->grRectL(aabb.vMin, aabb.vMax);
	glEnable(GL_TEXTURE_2D);
	Font->Print(text.c_str());
	return true;
}

#define Dclr RGBAf(0.1f, 0.1f, 0.1f, 0.0f)

bool CButton::Update(float dt)
{
	Vector2 mouse;
	CEngine::Instance()->GetState(STATE_MOUSE_XY, &mouse);

	switch (WidgetMouseState)
	{
	case wmsOutside:
		if (aabb.Inside(mouse))
			WidgetMouseState = wmsHovered;
		break;
	case wmsHovered:
		color += Dclr;
		WidgetMouseState = wmsInside;
		break;
	case wmsInside:
		if (!aabb.Inside(mouse))
		{
			WidgetMouseState = wmsLost;
			break;
		}
		if ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)))
		{
			color += Dclr;
			GuiManager.SetFocusedNodeTo(GuiManager.GetListNode(this));
			if (CallProc)
				CallProc();
			WidgetMouseState = wmsClicked;
		}
		break;
	case wmsLost:
		color -= Dclr;
		WidgetMouseState = wmsOutside;
		break;
	case wmsClicked:
		if (!(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)))
			WidgetMouseState = wmsReleased;
		break;
	case wmsReleased:
		color -= Dclr;
		if (aabb.Inside(mouse))
			WidgetMouseState = wmsInside;
		else
			WidgetMouseState = wmsOutside;
		break;

	}
	return true;
}

CButton::~CButton()
{

}
//////////////////////////////////////////////////////////////////////////
///CEdit

bool CEdit::InputHandling( Uint8 state, Uint16 key, SDLMod, char letter )
{
	switch(state)
	{
	case KEY_PRESSED:
		switch(key)
		{
		case SDLK_BACKSPACE:
			DelInterval(&text, CursorPos, CursorPos);
			break;
		case SDLK_DELETE:
			DelInterval(&text, CursorPos+1, CursorPos+1);
			break;
		case SDLK_LEFT:				
			if (--CursorPos <= -1)
				CursorPos++;
			break;
		case SDLK_RIGHT:
			if (++CursorPos >= text.length())
				CursorPos--;
			break;
		default:
			if (letter > 31)
			{
				text.insert(CursorPos, &letter, 1);
				CursorPos++;
			}
		}
		break;
	case KEY_RELEASED:
		switch(key)
		{
		case SDLK_AMPERSAND:
			break;
		default:;
		}
		break;
	}
	return true;

}

bool CEdit::Render()
{
	CEngine *engine = CEngine::Instance();
	Font = engine->FontManager->GetFont("Font");

	Font->Pos = (aabb.vMin + aabb.vMax) / 2.0f - Vector2(Font->GetStringWidth(text.c_str()), Font->GetStringHeight(text.c_str())) / 2.0f;
	Font->tClr = color;
	glLoadIdentity();
	PRender->grRectL(aabb.vMin, aabb.vMax);
	glEnable(GL_TEXTURE_2D);
	Font->Print(text.c_str());
	return true;
}

bool CEdit::Update( scalar dt )
{
	Vector2 mouse;
	CEngine::Instance()->GetState(STATE_MOUSE_XY, &mouse);

	switch (WidgetMouseState)
	{
	case wmsOutside:
		if (aabb.Inside(mouse))
			WidgetMouseState = wmsHovered;
		break;
	case wmsHovered:
		color += Dclr;
		WidgetMouseState = wmsInside;
		break;
	case wmsInside:
		if (!aabb.Inside(mouse))
		{
			WidgetMouseState = wmsLost;
			break;
		}
		if ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)))
		{
			color += Dclr;
			WidgetMouseState = wmsClicked;
		}
		break;
	case wmsLost:
		color -= Dclr;
		WidgetMouseState = wmsOutside;
		break;
	case wmsClicked:
		if (!(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)))
			WidgetMouseState = wmsReleased;
		break;
	case wmsReleased:
		color -= Dclr;
		if (aabb.Inside(mouse))
			WidgetMouseState = wmsInside;
		else
			WidgetMouseState = wmsOutside;
		break;
	}
	return true;

	return true;
}

CEdit::CEdit() : CursorPos(0)
{
}

CEdit::~CEdit()
{

}
//////////////////////////////////////////////////////////////////////////
///CMenuItem

#define DEFAULT_DISTANCE_BEETWEEN_ITEMS 20

CMenuItem::CMenuItem()
{
	FocusedOnItem = NULL;
	FocusedOnListNode = NULL;
	visible = false;
	isCycledMenuSwitch = true;
}

CMenuItem::CMenuItem(CMenuItem* AParent, char* AMenuText, Callback ACallProc)
{
	FocusedOnItem = NULL;
	FocusedOnListNode = NULL;
	visible = false;
	isCycledMenuSwitch = true;
	name = text = AMenuText;
	SetParent(AParent);
	if (AParent)
		AParent->AddObject(this);
	CallProc = ACallProc;
}

CMenuItem::~CMenuItem()
{

}

bool CMenuItem::Render()
{
	Reset();
	CMenuItem *ChildMenuItem = dynamic_cast<CMenuItem*>(Next());
	while (ChildMenuItem)
	{
		glLoadIdentity();
		Font->tClr = RGBAf(1.0,1.0,1.0,1.0);//ChildMenuItem->color;
		Font->scale = Vector2(1.0f, 1.0f);
		Font->Pos = ChildMenuItem->position;
		Font->Print(ChildMenuItem->text.c_str());
		ChildMenuItem = dynamic_cast<CMenuItem*>(Next());
	}
	glLoadIdentity();
	color = COLOR_WHITE;
	PRender->grCircleS(FocusedOnItem->position - Vector2(20.0f, -10.0f), 5);
	return true;
}

bool CMenuItem::Update(scalar dt)
{
	return true;
}

bool CMenuItem::InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
{
	if (!visible)
		return false;
	if (state == KEY_PRESSED)
	{
		switch(key)
		{
		case SDLK_UP:
			// Вероятно эту логику можно записать и покороче TODO
			if (FocusedOnListNode == GetFirst() && isCycledMenuSwitch)
			{
				FocusedOnListNode = GetLast();
				if (FocusedOnListNode)
					FocusedOnItem = dynamic_cast<CMenuItem*>(FocusedOnListNode->GetData());
				break;
			}
			FocusedOnListNode = RelativePrev(FocusedOnListNode);
			if (FocusedOnListNode)
				FocusedOnItem = dynamic_cast<CMenuItem*>(FocusedOnListNode->GetData());
			break;
		case SDLK_DOWN:
			if (FocusedOnListNode == GetLast() && isCycledMenuSwitch)
			{
				FocusedOnListNode = GetFirst();
				if (FocusedOnListNode)
					FocusedOnItem = dynamic_cast<CMenuItem*>(FocusedOnListNode->GetData());
				break;
			}
			FocusedOnListNode = RelativeNext(FocusedOnListNode);
			if (FocusedOnListNode)
				FocusedOnItem = dynamic_cast<CMenuItem*>(FocusedOnListNode->GetData());
			break;
		case SDLK_RETURN:
			if (FocusedOnItem->CallProc)
				FocusedOnItem->CallProc();
			else
				if (FocusedOnItem->GetFirst())
				{
					visible = false;
					FocusedOnItem->visible = true;
					GuiManager.SetFocus(FocusedOnItem);
				}
			break;
		case SDLK_ESCAPE:
			if (!Parent)
				break;
			visible = false;
			(dynamic_cast<CMenuItem*>(Parent))->visible = true;
			GuiManager.SetFocus(dynamic_cast<CMenuItem*>(Parent));
			break;
		}
	}
	else
	{

	}
	return true;
}

bool CMenuItem::AddObject(CObject *AObject)
{
	CList::AddObject(AObject);
	FocusedOnListNode = GetFirst();
	if (FocusedOnListNode)
		FocusedOnItem = dynamic_cast<CMenuItem*>(FocusedOnListNode->GetData());
	return true;
}
