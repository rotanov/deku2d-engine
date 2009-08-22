#include "OpenglUtils.h"
#include "Engine.h"

void CRenderObject::SetColor( byte _r, byte _g, byte _b, byte _a )
{
	color.r = _r;
	color.g = _g;
	color.b = _b;
	color.a = _a;
}

CRenderObject::CRenderObject() : p(V2Zero), depth(0.0f), visible(true), color(1.0f, 1.0f, 1.0f, 1.0f)
{
	name += "CRenderObject ";
	type |= T_RENDERABLE;
};

//-------------------------------------------//
//			CGLImageData stuff				 //
//-------------------------------------------//
CGLImageData::CGLImageData()
{
	TexID = width = height = bpp = 0;
	isDrawing = false;
	CleanData = true;
	data = NULL;
}

bool CGLImageData::MakeTexture()
{
	if (data == NULL) 
		return false;
	if ((width&(width-1)) != 0) 
		return false;
	if ((height&(height-1)) != 0) 
		return false;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	int MODE = (bpp == 3)?GL_RGB : GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, bpp, width, height, 0, MODE, GL_UNSIGNED_BYTE, data);
	return true;
}


bool CGLImageData::LoadTexture(const char *filename)
{
	if (!LoadBMP(filename))
		if(!LoadPNG(filename))
		{
			Log("ERROR", "Can't load image->");
			return false;
		}
		else
		{
			if (!MakeTexture())
			{
				Log("ERROR", "Can't load texture in video adapter.");
				return false;
			}
		}
	else
	{
		if(!MakeRGBA())
		{
				Log("ERROR", "Can't load texture.");
				return false;
		}
		if (!MakeTexture())
		{
			Log("ERROR", "Can't load texture in video memory.");
			return false;
		}
	}
	if (CleanData)
	{
		delete [] data;
		data = NULL;
	}
	return true;
}

bool CGLImageData::RenderWhole(int zoom)
{
	if (TexID == 0)
	{
		Log("ERROR", "Trying render, while texture has not been loaded.");
		return false;
	}
	glPushAttrib(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f,			0.0f		);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(width*zoom,	0.0f		);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(width*zoom,	height*zoom	);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f,			height*zoom	);
	glEnd();
	glPopAttrib();
	return true;
}


// В жоа такие ф-ии. Убрать нахуй
bool CGLImageData::BeginDraw()
{
	if (isDrawing)
	{
		Log("ERROR", "Illegal BeginDraw() method call. EndDraw() should be called before.");
		return false;
	}
	if (TexID == 0)
	{
		Log("ERROR", "Trying render, while texture has not been loaded.");
		return false;
	}
	isDrawing = true;
	glPushAttrib(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glLoadIdentity();
	glBegin(GL_QUADS);
	return true;
}

bool CGLImageData::PushQuad(float x0, float y0, float z0, float _width, float _height, int s0, int t0, int s1, int t1)
{
	if (!isDrawing)
	{
		Log("ERROR", "Illegal PushQuad method call. BeginDraw() should be called before.");
		return false;
	}
	glTexCoord2f((float)s0/width, (float)t0/height); glVertex3f(x0, y0, z0);
	glTexCoord2f((float)s1/width, (float)t0/height); glVertex3f(x0+_width, y0, z0);
	glTexCoord2f((float)s1/width, (float)t1/height); glVertex3f(x0+_width, y0+_height, z0);
	glTexCoord2f((float)s0/width, (float)t1/height); glVertex3f(x0, y0+_height, z0);
	return true;
}
/*Don't touch it!!!! only for Gui(may be xP)*/
bool CGLImageData::PushQuadEx(float x0, float y0, float z0, float _width, float _height, int s0, int t0, int s1, int t1, bool tiled)
{
	if (!isDrawing)
	{
		Log("ERROR", "Illegal PushQuadEx method call. BeginDraw() should be called before.");
		return false;
	}

	glTexCoord2f((float)s0/width, (float)(height-t0)/height); 
//	glVertex3f(x0, scrheight-y0, z0);


	glTexCoord2f((float)(s1 + s0)/width, (float)(height-t0)/height); 
//	glVertex3f(x0+_width,  scrheight-y0, z0);


	glTexCoord2f((float)(s1 + s0)/width, (float)(height-t1-t0)/height); 
//	glVertex3f(x0+_width,  scrheight-(y0+_height), z0);


	glTexCoord2f((float)s0/width, (float)(height-t1-t0)/height); 
//	glVertex3f(x0,  scrheight-(y0+_height), z0);


	return true;
}

bool CGLImageData::EndDraw()
{
	if (!isDrawing)
	{
		Log("ERROR", "Illegal EndDraw() method call. BeginDraw() should be called before.");
		return false;
	}
	isDrawing = false;
	glEnd();
	glPopAttrib();
	return true;
}

GLuint CGLImageData::GetTexID()
{
	if (TexID == 0)
		Log("ERROR", "GLImageData. Trying to access TexID but it is 0");
	return TexID;
}
//-------------------------------------------//
//				CSprite functions			 //
//-------------------------------------------//
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
	file.Open(filename, CFILE_READ);
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
	file.Open(filename, CFILE_WRITE);
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
//-------------------------------------------//
//				CGLWindow functions			 //
//-------------------------------------------//

CGLWindow::CGLWindow()
{
	width = 640;
	height = 480;
	fullscreen = false;
	bpp = 32;
	caption = "Warning: CGLWindow class instance have not been initialized";
}
CGLWindow::~CGLWindow(){}

bool CGLWindow::gCreateWindow(int _width, int _height, byte _bpp, char* _caption)
{
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 )
	{
		Log("ERROR", "Video initialization failed: %s\n", SDL_GetError());
		Log("ERROR", "Last WARNING was critical. Now exiting...");
		SDLGLExit(1);
	}

	width = _width;
	height = _height;
	bpp = _bpp;
	if (caption == NULL && _caption != NULL)
	{
		caption = new char [strlen(_caption)];
		strcpy(caption, _caption);
	}
	SDL_WM_SetCaption(caption, NULL);
	const SDL_VideoInfo* info = NULL;

	info = SDL_GetVideoInfo();

	if (info == NULL)
	{
		Log("ERROR", "Aaaa! SDL WARNING: %s", SDL_GetError());
		Log("ERROR", "Last WARNING was critical. Now exiting...");
		SDLGLExit(1);
	}

	

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,		8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,		8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,		8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,		16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,	1);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,		8);
// 	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
// 	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);
// 
// 	glEnable(0x809D);

	int flags = SDL_OPENGL;
	if(fullscreen == true)
	{
		flags |= SDL_FULLSCREEN;
	}

	SDL_Surface * screen = SDL_SetVideoMode(width, height, bpp, flags);
	if (screen == NULL)
	{
		Log("ERROR", "Setting video mode failed: %s\n", SDL_GetError());
		Log("ERROR", "Last WARNING was critical. Now exiting...");
		SDLGLExit(1);		
	}

	SDL_Event resizeEvent;
	resizeEvent.type=SDL_VIDEORESIZE;
	resizeEvent.resize.w=width;
	resizeEvent.resize.h=height;
	SDL_PushEvent(&resizeEvent);
	
	glInit(width, height);

	CPrimitiveRender temp;
	temp.Init();
	return true;
}

bool CGLWindow::gCreateWindow()
{
	return gCreateWindow(width, height, bpp, caption);
}

bool CGLWindow::glInitSystem()
{
	return true;
}

void CGLWindow::glSuicide()
{
}

void CGLWindow::glResize(GLsizei Width, GLsizei Height)
{
	if (Height == 0)		
		Height = 1;
	glViewport(0, 0, Width-1, Height-1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();			
	gluOrtho2D(0.0f, Width-1, 0.0f, Height-1);
	glMatrixMode(GL_MODELVIEW);	
}

void CGLWindow::glInit(GLsizei Width, GLsizei Height)	
{
	glShadeModel(GL_SMOOTH);	//GL_SMOOTH GL_FLAT
	
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearColor(0.0, 0.0, 0.0, 0.5);
	
	glEnable(GL_BLEND);

	if (Height == 0)		
		Height = 1;
	if (Width == 0)		
		Width = 1;
	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();			
	gluOrtho2D(0.0f, Width - 1, 0.0f, Height - 1);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);	

		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	 	
	 	
	 	


	//strstr(WGL_EXT_swap_control);
	setVSync(1);
}

//-------------------------------------------//
//				Font stuff					 //
//-------------------------------------------//
CFont::CFont()
{
	scale = Vector2(1.0f, 1.0f);
	base = 0;
	Distance = CFONT_DEFAULT_DISTANCE; // Нет! Грузить её из файла!!! Ну, по крайне мере по умолчанию ставить из файла значение. Пользователь потом сам попроавит, если надо.
	pp = &Pos;
	offset = 0;
	isTextSelected = doRenderToRect = false;
	Sel0 = Sel1 = 0;
	tClr = RGBAf(1.0f, 1.0f, 1.0f, 1.0f);	// MAGIC NUMBER
	SetDepth(0.0f);
	CFontManager* FntMan = CFontManager::Instance("OpenglUtils.cpp");
	FntMan->AddObject(this);
	FntMan->FreeInst("OpenGLutils.cpp");
}

CFont::~CFont()
{
	if (!base)
		glDeleteLists(base, 256);
}


bool CFont::LoadFromFile()
{
	if (filename == "")
		return false;
	CFile			file;
	if (!file.Open(filename.c_str(), CFILE_READ))
	{
		Log("ERROR","Can't Load Font %s: file  couldn't be opened.", name.data()); //TODO: filename wrte too.
		return false;
	}
	char *FontImageName = NULL;
	file.ReadLine(FontImageName);	
	CTextureManager *TexMan = CTextureManager ::Instance();
	Texture = TexMan->GetTextureByName(FontImageName);
	TexMan->FreeInst();
	file.Read(bbox, sizeof(bbox));
	file.Close();
	base = glGenLists(256);
	glBindTexture(GL_TEXTURE_2D, Texture->GetTexID());
	float TmpOOWdth = 1.0f/Texture->width, TmpOOHght = 1.0f/Texture->height;
	for(int i=0;i<256;i++)
	{
		glNewList(base+i, GL_COMPILE);
		glBegin(GL_QUADS);		
			glTexCoord2f( (float)bbox[i].x0*TmpOOWdth, (float)bbox[i].y0*TmpOOHght);
			glVertex2i(0, 0);

			glTexCoord2f( (float)bbox[i].x1*TmpOOWdth, (float)bbox[i].y0*TmpOOHght);
			glVertex2i(bbox[i].x1 - bbox[i].x0, 0);

			glTexCoord2f( (float)bbox[i].x1*TmpOOWdth, (float)bbox[i].y1*TmpOOHght);
			glVertex2i(bbox[i].x1 - bbox[i].x0, bbox[i].y1 - bbox[i].y0);

			glTexCoord2f( (float)bbox[i].x0*TmpOOWdth, (float)bbox[i].y1*TmpOOHght);
			glVertex2i(0, bbox[i].y1 - bbox[i].y0);
		glEnd();
		glTranslated(bbox[i].x1 - bbox[i].x0 + Distance, 0, 0);
		glEndList();
		width[i] = (bbox[i].x1 - bbox[i].x0);
		height[i] = (bbox[i].y1 - bbox[i].y0);
	}
	return true;
}

bool CFont::SaveToFile()
{
	if (filename == "")
		return false;
	if (!Texture)
		return false;
	CFile file;
	file.Open(filename.c_str(), CFILE_WRITE);
	file.Write(Texture->name.c_str(), (DWORD)Texture->name.length());
	file.WriteByte((byte)0x00);
	file.Write(bbox, sizeof(bbox));
	file.Close();
	return true;
}

void CFont::_Print(const char *text)
{
	glPushAttrib(GL_TEXTURE_BIT);
	glPushMatrix();
	tClr.glSet();
	glEnable(GL_TEXTURE_2D);
	Texture->Bind();
	glTranslatef(pp->x, pp->y, Depth);
	scale.glScale();
	glListBase(base-32);
	glCallLists((GLsizei)strlen(text), GL_BYTE, text);
	glPopMatrix();
	glPopAttrib(); 
	//GLenum err = glGetError() ;
}




void CFont::Print( const char *text, ... )
{
	if (text == NULL)
		return;
	if ((uint)offset >= strlen(text) || offset < 0)
		return;

	int swidth = 0, sheight = 0, selx = 0;
	if (isTextSelected)
	{
		if (Sel0 > Sel1+1)
		{
			swap(Sel0, Sel1);
			Sel0++;
			Sel1--;
		}
		
		swidth = GetStringWidth(text + offset);
		sheight = GetStringHeight(text + offset);
		selx = Pos.x + GetStringWidthEx(0, Sel0-1-offset, text+offset);


		if (!(Sel0 > Sel1 || Sel0 < 0 || (uInt)Sel1 >= strlen(text)))
		{
			int selw = GetStringWidthEx(max(Sel0, offset), Sel1, text);
			CPrimitiveRender pr;
			pr.psClr = &RGBAf(10, 50, 200, 150);
			pr.depth = Depth;
			pr.grRectS(Vector2(selx, Pos.y), Vector2(selx + selw, Pos.y + sheight));
		}


	}

		int xpos, ypos;
	if (doRenderToRect)
	{
		
		byte tempV = align & CFONT_VALIGN_MASK, tempH = align & CFONT_HALIGN_MASK;

		if (tempV == CFONT_VALIGN_TOP)
		{
			ypos = Pos.y + aabb.y0 - sheight;
		}
		if (tempV == CFONT_VALIGN_BOTTOM)
		{
			ypos = Pos.y;
		}
		if (tempV == CFONT_VALIGN_CENTER)
		{
			ypos = Pos.y + aabb.y0/2 - sheight/2;
		}

		if (tempH == CFONT_HALIGN_LEFT)
		{
			xpos = Pos.x;
		}
		if (tempH == CFONT_HALIGN_RIGHT)
		{
			xpos = Pos.x + aabb.x0 - swidth;
		}

		if (tempH == CFONT_HALIGN_CENTER)
		{
			xpos = Pos.x + aabb.x0/2 - swidth/2;
		}

		glPushAttrib(GL_SCISSOR_TEST);
		glEnable(GL_SCISSOR_TEST);
		glScissor(Pos.x, Pos.y, aabb.x0, aabb.y0);


		CPrimitiveRender pr;
		pr.depth = Depth;
		pr.lClr = RGBAf(0.4f, 0.5f, 0.7f, 0.9f);
		pr.sClr = RGBAf(0.5f, 0.7f, 0.4f, 0.9f);
		pr.pClr = RGBAf(0.7f, 0.5f, 0.4f, 0.9f);

 		pr.grRectS(Vector2(selx - 1, ypos), Vector2(selx - 1 + 1, ypos + min(sheight, GetStringHeight("!\0")))); //  glRGBAub(10, 10, 10, 200)
		pr.grRectS(Vector2(selx - 3, ypos), Vector2(selx - 3 + 5, ypos + 1)); // glRGBAub(10, 10, 10, 200)
 			//gSolidRectEx(selx - 2, ypos, 5, 1, depth, &glRGBAub(10, 10, 10, 200));
 		pr.grRectS(Vector2(selx - 3, ypos + min(sheight, GetStringHeight("!\0"))), Vector2(selx - 3 + 5,ypos + min(sheight, GetStringHeight("!\0"))+ 1)); // glRGBAub(10, 10, 10, 200)

	}


	static char	*temp;//[CFONT_MAX_STRING_LENGTH];
	temp = new char [strlen(text)+CFONT_MAX_STRING_LENGTH];
	va_list	ap;
	va_start(ap, text);
	vsprintf(temp, text, ap);
	va_end(ap);
	_Print(temp);
	delete [] temp;

	if (doRenderToRect)
	{
		 	glDisable(GL_SCISSOR_TEST);
		 	glPopAttrib();
	}
}


int	CFont::GetStringWidth(const char *text)
{
	
	if (text == NULL)
		return -1;
	int r = 0, l = (int)strlen(text);
	for (int i=0;i<l;i++)
		r += width[text[i]-32] + Distance;
	return r;
}

int CFont::GetStringWidthEx(int t1, int t2, const char *text)
{
	if (text == NULL)
		return -1;
	if (t1 > t2 || t2 < 0)
		return -1;
	if ((uint)t2 >= strlen(text))
		return -1;
	int r = 0;
	for (uint i = t1; i <= t2; i++)
		r += width[text[i]-32]+Distance;
	return r;
}

int	CFont::GetStringHeight(const char *text)
{
	if (text == NULL)
		return -1;
	int r = 0, l = (uInt)strlen(text);
	for (int i=0;i<l;i++)
		r = Max(height[text[i]-32], r);
	return r;
}

int CFont::GetStringHeightEx( int t1, int t2, const char *text )
{
	if (text == NULL)
		return -1;
	if (t1 > t2 || t2 < 0)
		return -1;
	if ((uint)t2 >= strlen(text))
		return -1;
	int r = 0, l = (uint)strlen(text);
	for (uint i=0; i<l; i++)
		r = Max(height[text[i]-32], r);
	return r;
}

void CFont::SetDepth( float _depth )
{
	Depth = _depth;
	Depth = clampf(Depth, CFONT_DEPTH_LOW, CFONT_DEPTH_HIGH);
}

void CFont::PointTo(const Vector2 *_p)
{
	if (!_p)
		return;
	pp = _p;
}

void CFont::PointBack()
{
	pp = &Pos;
}

void CFont::SetAlign( const byte _Halign, const byte _Valign )
{
	align = _Valign || _Halign;
}

byte CFont::GetHalign()
{
	return align & CFONT_HALIGN_MASK;
}

byte CFont::GetValign()
{
	return align & CFONT_VALIGN_MASK;
}

void CFont::AssignTexture(CTexture* AFontTexture)
{
	if (AFontTexture)
		Texture = AFontTexture;
}
//-------------------------------------------//
//				CRenderManager				 //
//-------------------------------------------//
CRenderManager::CRenderManager()
{
	v2Dots		= NULL;
	v2Lines		= NULL;
	v2Quads		= NULL;
	v2Triangles	= NULL;
}

CRenderManager::~CRenderManager()
{

}

bool CRenderManager::DrawObjects()
{
	Reset();
	CRenderObject *data = NULL;//dynamic_cast<CRenderObject*>(Next());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	Camera.Update();
	
	
	while (Enum((CObject*&)data))
	{
		if (data->visible)
		{
			glLoadIdentity();
				Camera.gTranslate();
			glTranslatef(data->p.x, data->p.y, data->depth);
			data->Render();
		}
		//data = dynamic_cast<CRenderObject*>(Next());
	}
	glPopMatrix();

	#ifdef _DEBUG
	//Camera.DrawDebug();
	#endif 

	return true;
}

bool CompAlpha(CObject *a, CObject *b)
{
	return (dynamic_cast<CRenderObject*>(a))->color.a >= (dynamic_cast<CRenderObject*>(b))->color.a;
}

bool CompZ(CObject *a, CObject *b)
{
	return (dynamic_cast<CRenderObject*>(a))->depth <= (dynamic_cast<CRenderObject*>(b)->depth);
}


void CRenderManager::SortByAlpha()
{
	this->Sort(CompAlpha);
}

void CRenderManager::SortByZ()
{
	this->Sort(CompZ);
}

//-------------------------------------------//
//				Common OpenGL stuff			 //
//-------------------------------------------//

void gBeginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gEndFrame()
{
	glFinish();
	SDL_GL_SwapBuffers();
}

void gSetBlendingMode(void)
{
		glEnable			(GL_BLEND);
		glBlendFunc			(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//(GL_SRC_ALPHA,GL_ONE)
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable			(GL_ALPHA_TEST);
		glAlphaFunc			(GL_GREATER, 0);
}


void SDLGLExit(int code)
{
	SDL_Quit();
    exit(0x01);
}
//-------------------------------------------//
//			Particle system stuff			 //
//-------------------------------------------//

void gToggleScissor( bool State )
{
	if (State)
	{
		glEnable(GL_SCISSOR_TEST);
//		Log("GLINFO", "Enabled Scissor test");
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
//		Log("GLINFO", "Disabled Scissor test");
	}
}

void gScissor( int x, int y, int width, int height )
{
	glScissor(x, y, width, height);
}

void gSetColor( RGBAf color )
{
	glColor4fv(color.v);
	//glColor4f(color.r, color.g, color.b, color.a);
}

void gDrawBBox( CAABB box )
{
	glPushMatrix();
	glPushAttrib(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	glEnable(GL_LINE_WIDTH);
	glLineWidth(4);
	glTranslatef(0.0f, 0.0f, 0.99f);
	glBegin(GL_LINE_LOOP);
		glVertex2f(box.vMin.x, box.vMin.y);	
		glVertex2f(box.vMax.x, box.vMin.y);
		glVertex2f(box.vMax.x, box.vMax.y);
		glVertex2f(box.vMin.x, box.vMax.y);
	glEnd();
	glPopAttrib();
	glPopMatrix();
}
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
			gSetColor(particles[i].c);
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
	file.Open(filename.c_str(), CFILE_WRITE);
	file.Write(&Info, sizeof(CPsysteminfo));
	file.Close();
	return true;
}

bool CParticleSystem::LoadFromFile()
{
	CFile file;
	file.Open(filename.c_str(), CFILE_READ);
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
//			Font Manager
//////////////////////////////////////////////////////////////////////////

CFontManager::~CFontManager()
{

}

CFontManager::CFontManager()
{
	CurrentFont = NULL;
}

CFontManager* CFontManager::Instance(char *byWho)
{
	if (_instance == NULL)
	{
		_instance = new CFontManager;
		Log("INFO", "Memory allocated for FontManager");
	}
	_refcount++;
	Log("INFO", "Font manager instanced by %s", byWho);
	return _instance;
}


void CFontManager::FreeInst(char* byWho)
{
	_refcount--;
	if (!_refcount)
	{		
		delete this;
		Log("INFO", "FontManager deleted from memory by %s.", byWho);
		_instance = NULL;
	}
	Log("Info", "FontManager FreeInst by %s.", byWho);
}

CFontManager* CFontManager::_instance = 0;
int CFontManager::_refcount = 0;


CFont* CFontManager::GetFont(const char* fontname )	
{
	CFont *TempFont = NULL;
	TempFont = dynamic_cast<CFont*>(GetObjectByName(fontname));
	if (TempFont)
		TempFont->CheckLoad();
	return TempFont;
}

CFont* CFontManager::GetFontEx( string fontname )
{
	return dynamic_cast<CFont*>(GetObjectByName(fontname.c_str()));
}

bool CFontManager::SetCurrentFont( char* fontname )
{
	CurrentFont = GetFont(fontname);
	if (!CurrentFont->loaded)
		CurrentFont->LoadFromFile();
	return !!CurrentFont;
}

bool CFontManager::Print( int x, int y, float depth, char* text, ... )
{
	if (!CurrentFont)

		return false;

	CurrentFont->SetDepth(depth);
	CurrentFont->Pos.In(x, y);
	CurrentFont->Print(text);
	return true;
}

bool CFontManager::PrintEx( int x, int y, float depth, char* text, ... )
{
	if (!CurrentFont)
		return false;

	char temp[256];
	va_list	ap;
	va_start(ap, text);
	vsprintf(temp, text, ap);
	va_end(ap);

	CurrentFont->SetDepth(depth);
	CurrentFont->Pos.In(x, y);
	CurrentFont->Print(temp);
	return true;
}

bool CFontManager::AddObject( CObject *object )
{
	CList::AddObject(object);
	if (CurrentFont == NULL)
		CurrentFont = dynamic_cast<CFont*>(object);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//						CTexture Manager
//////////////////////////////////////////////////////////////////////////

CTextureManager* CTextureManager::Instance()
{
	if (_instance == NULL)
	{
		_instance = new CTextureManager;
	}
	_refcount++;
	return _instance;
}

void CTextureManager::FreeInst()
{
	_refcount--;
	if (!_refcount)
	{
		delete this;
		Log("INFO", "TextureManager deleted from memory.");
		_instance = NULL;
	}
}

CTextureManager::CTextureManager()
{
	name = "TextureManager";
}

CTextureManager::~CTextureManager()
{

}

bool CTextureManager::AddTexture( char* filename, bool load)
{
	CTexture *tempTRes = new CTexture(filename);
	if (load)
		tempTRes->Load();
	AddObject(tempTRes);
	return true;
}

bool CTextureManager::LoadTextureByName(char *TextureName)
{
	CTexture *tmp = NULL;
	tmp = dynamic_cast<CTexture*>(GetObjectByName(TextureName));
	if (tmp == NULL)
	{
		Log("ERROR", "Can't load find texture with name %s", TextureName);
		return false;
	}
	return tmp->Load();
}

bool CTextureManager::LoadAllTextures()
{
	CTexture *tmp = NULL;
	this->Reset();
	while (this->Enum((CObject*&)tmp))
	{	
		if (tmp == NULL)
		{
			Log("ERROR", "Can't find some texture");
			return false;
		}
		tmp->Load();
		tmp = NULL;
	}
}

CTexture* CTextureManager::GetTextureByName( const string &TextureName )
{
	CTexture *TempTexture = NULL;
	TempTexture = dynamic_cast<CTexture*>(GetObjectByName(TextureName.c_str()));
	if (TempTexture)
		TempTexture->CheckLoad();
	return TempTexture;
}
CTextureManager* CTextureManager::_instance = 0;
int CTextureManager::_refcount = 0;


//////////////////////////////////////////////////////////////////////////
//					CTexture
//////////////////////////////////////////////////////////////////////////
bool CTexture::Load()
{
	if (filename == "")
	{
		Log("ERROR", "Trying to load texture with name %s; But it has not been found in ResourceList(s)\n\t or Resource List Has not been loaded", name);
		return false;
	}
	if (!loaded)
		if (LoadTexture(filename.c_str()))
			loaded = true;
	return loaded;
}

GLuint CTexture::GetTexID()
{
	if (TexID == 0)
	{
		Log("ERROR", "CTextuere named %s. Trying to access TexID but it is 0", name.c_str());
		if (!loaded)
		{
			Load();
		}		
	}
	return TexID;
}

CTexture::CTexture(char * vfilename)
{	
	filename = vfilename;
	name = "CTexture";
	CTextureManager *TexMan = CTextureManager::Instance();
		TexMan->AddObject(this);
	TexMan->FreeInst();
}

CTexture::CTexture()
{
	name = "CTexture";
	CTextureManager *TexMan = CTextureManager::Instance();
	TexMan->AddObject(this);
	TexMan->FreeInst();
}

void CTexture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, GetTexID());
}

void CTexture::Unload()
{
	glDeleteTextures(1, &TexID);
}

bool CTexture::LoadFromFile()
{
	Load();
	return true;
}
void CCamera::Assign(scalar *x, scalar *y)
{
	CEngine* engine = CEngine::Instance();  // too mush routines // укоротить
	engine->GetState(STATE_SCREEN_HEIGHT, &h);
	engine->GetState(STATE_SCREEN_HEIGHT, &w);
	engine->FreeInst();
	Atx = x;
	Aty = y;
	dx = *x;
	dy = *y;
	Point.x = 0;//- dx; //w/2.0f 
	Point.y = 0;//- dy; //h/2.0f
	Assigned = true;
}

void CCamera::Update()
{
	if (!Assigned)
		return;
	//dx -= *Atx;
	//dy -= *Aty;	
/*
	world = view.Offsetted(Point.x, Point.y);

	Vector2 n;
	scalar d;
	if (world.Outside(Vector2(*Atx, *Aty), d, n))
	{
		
		//v = Vector2(*Atx, *Aty) - Vector2(dx, dy);//Point;
		v = n*d;
		Point += v*0.022f;



		glTranslatef(-Point.x, -Point.y, 0.0f);
		return;

	}
*/
	v = Vector2(*Atx, *Aty) - Point;
	if (abs(v.x) < 0.001f && abs(v.y) < 0.001f)
		v = Vector2::Blank();
		Point += v*0.05f;

	dx = *Atx;
	dy = *Aty;

	//glTranslatef(-Point.x + w/2.0f, -Point.y + h/2.0f, 0.0f);
}

void CCamera::Free()
{
	Atx = NULL;
	Aty = NULL;
	Assigned = false;
}

void CCamera::DrawDebug()
{
	glEnable(GL_BLEND);
	glColor4f(0.8f, 0.4f, 0.3f, 0.5f);
	gDrawBBox(world);
	glColor4f(0.4f, 0.8f, 0.3f, 0.5f);
	gDrawBBox(view);
	glColor4f(0.3f, 0.4f, 0.8f, 0.5f);
	gDrawBBox(outer);
}

void CCamera::gTranslate()
{
	glTranslatef((int) (-Point.x + w/2.0f), (int) (-Point.y + h/2.0f), 0.0f);
}

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Primitive Render Section                                             */
/************************************************************************/

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
	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObjectByName("cells"));
	Tman->FreeInst();
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
	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObjectByName("cells"));
	Tman->FreeInst();
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
#ifdef WIN32

#include <windows.h>

PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

void setVSync(int interval)
{
	const char *extensions = (const char *)glGetString(GL_EXTENSIONS);

	if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
	{
		Log("Error", "WGL_EXT_swap_control extension not supported on your computer.");
		return;  
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if(wglSwapIntervalEXT)
			wglSwapIntervalEXT(interval);
	}
}

#endif

CGUIObjectMini::CGUIObjectMini()
{
	CEngine::Instance()->RenderManager.AddObject(this);
	CEngine::Instance()->UpdateManager.AddObject(this);	
	GuiManager.AddObject(this);
	font = CEngine::Instance()->FontManager->GetFont("Font"); // Это конструкто же, а что если конструируется до инициализации движка? втф...
	PRender = new CPrimitiveRender;
	PRender->plClr = &color;
	PRender->psClr = &color;
	PRender->ppClr = &color;
	// Mohawk!!!!
	// TODO
}


CButtonMini::CButtonMini( CAABB ARect, char* AText, RGBAf AColor, Callback AOnClick ):
	OnClick(AOnClick), state(bsOutside)
{
	aabb = ARect;
	color = AColor;
	text = AText;

	PRender->DashedLines = true;
	PRender->dash = 0x0001;
}

bool CButtonMini::Render()
{	
	CEngine *engine = CEngine::Instance();
	font = engine->FontManager->GetFont("Font");
	engine->FreeInst();
	font->Pos = (aabb.vMin + aabb.vMax) / 2.0f - Vector2(font->GetStringWidth(text.c_str()), font->GetStringHeight(text.c_str())) / 2.0f;
	font->tClr = color;
	glLoadIdentity();
	PRender->grRectL(aabb.vMin, aabb.vMax);
	glEnable(GL_TEXTURE_2D);
	font->Print(text.c_str());
	return true;
}

#define Dclr RGBAf(0.1f, 0.1f, 0.1f, 0.0f)

bool CButtonMini::Update(float dt)
{
	Vector2 mouse;
	CEngine::Instance()->GetState(STATE_MOUSE_XY, &mouse);

	switch (state)
	{
		case bsOutside:
			if (aabb.Inside(mouse))
				state = bsHovered;
			break;
		case bsHovered:
			color += Dclr;
			state = bsInside;
			break;
		case bsInside:
			if (!aabb.Inside(mouse))
			{
				state = bsLost;
				break;
			}
			if ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)))
			{
				color += Dclr;
				GuiManager.SetFocusedNodeTo(GuiManager.GetListNodeByPointerToObject(this));
				if (OnClick)
					OnClick();
				state = bsClicked;
			}
			break;
		case bsLost:
			color -= Dclr;
			state = bsOutside;
			break;
		case bsClicked:
			if (!(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)))
				state = bsReleased;
			break;
		case bsReleased:
				color -= Dclr;
				if (aabb.Inside(mouse))
					state = bsInside;
				else
					state = bsOutside;
			break;

	}
	return true;
}

CGUIManagerMini::CGUIManagerMini(): KeyHoldRepeatInterval(50), KeyHoldRepeatDelay(300), tabholded(false), TimerAccum(0)
{
	name = "Mini GUI manager";
	FocusedOn = NULL;
	CEngine::Instance()->UpdateManager.AddObject(this);
	CEngine::Instance()->RenderManager.AddObject(this);
	CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
}

bool CGUIManagerMini::Update( scalar dt )
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

bool CGUIManagerMini::Render()
{
	glLoadIdentity();
	if (FocusedOn)
	{
		FocusedOn->PRender->LineStippleEnabled = true;
		FocusedOn->PRender->lwidth = 0.5f;
		FocusedOn->PRender->grRectL(FocusedOn->aabb.Inflated(5, 5).vMin, FocusedOn->aabb.Inflated(5, 5).vMax);	
		FocusedOn->PRender->LineStippleEnabled = false;
		FocusedOn->PRender->lwidth = 1.0f;
	}
		//FocusedOn->PRender->gDrawBBox(FocusedOn->aabb.Inflated(5, 5));
	return true;
}

bool CGUIManagerMini::InputHandling( Uint8 state, Uint16 key, SDLMod mod, char letter )
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
					FocusedOn = dynamic_cast<CGUIObjectMini*>(GetFirst()->GetData());
			}
			else
			{
				FocusedOnListNode = FocusedOnListNode->next;
				if (FocusedOnListNode)
					FocusedOn = dynamic_cast<CGUIObjectMini*>(FocusedOnListNode->GetData());

			}

			if (FocusedOnListNode == NULL)
			{
				FocusedOnListNode = GetFirst();
				if (GetFirst())
					FocusedOn = dynamic_cast<CGUIObjectMini*>(GetFirst()->GetData());
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

void CGUIManagerMini::SetFocusedNodeTo(CListNode* AFocusedNode)
{
	FocusedOnListNode = AFocusedNode;
	if (FocusedOnListNode)
		FocusedOn = dynamic_cast<CGUIObjectMini*>(FocusedOnListNode->GetData());
}

CGUIManagerMini GuiManager;

bool CEditMini::InputHandling( Uint8 state, Uint16 key, SDLMod, char letter )
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

bool CEditMini::Render()
{
	CEngine *engine = CEngine::Instance();
	font = engine->FontManager->GetFont("Font");
	engine->FreeInst();
	font->Pos = (aabb.vMin + aabb.vMax) / 2.0f - Vector2(font->GetStringWidth(text.c_str()), font->GetStringHeight(text.c_str())) / 2.0f;
	font->tClr = color;
	glLoadIdentity();
	PRender->grRectL(aabb.vMin, aabb.vMax);
	glEnable(GL_TEXTURE_2D);
	font->Print(text.c_str());
	return true;
}

bool CEditMini::Update( scalar dt )
{
	Vector2 mouse;
	CEngine::Instance()->GetState(STATE_MOUSE_XY, &mouse);

	switch (state)
	{
	case bsOutside:
		if (aabb.Inside(mouse))
			state = bsHovered;
		break;
	case bsHovered:
		color += Dclr;
		state = bsInside;
		break;
	case bsInside:
		if (!aabb.Inside(mouse))
		{
			state = bsLost;
			break;
		}
		if ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)))
		{
			color += Dclr;
			state = bsClicked;
		}
		break;
	case bsLost:
		color -= Dclr;
		state = bsOutside;
		break;
	case bsClicked:
		if (!(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)))
			state = bsReleased;
		break;
	case bsReleased:
		color -= Dclr;
		if (aabb.Inside(mouse))
			state = bsInside;
		else
			state = bsOutside;
		break;
	}
	return true;

	return true;
}

CEditMini::CEditMini() : CursorPos(0)
{
}