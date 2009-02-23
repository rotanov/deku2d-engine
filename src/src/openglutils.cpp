#include "OpenglUtils.h"
//-------------------------------------------//
//				Colors stuff				 //
//-------------------------------------------//
glRGBAub::glRGBAub(byte _r, byte _g, byte _b, byte _a)
{
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

glRGBAub::glRGBAub()
{
	r = 0;
	g = 0;
	b = 0;
	a = 255;
}

void glRGBAub::glSet()
{
	glColor4ubv(&r);
}

void CRenderObject::SetColor( byte _r, byte _g, byte _b, byte _a )
{
	color.r = _r;
	color.g = _g;
	color.b = _b;
	color.a = _a;
}

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
			Log("AHTUNG", "Can't load image->");
			return false;
		}
		else
		{
			if (!MakeTexture())
			{
				Log("AHTUNG", "Can't load texture in video adapter.");
				return false;
			}
		}
	else
	{
		if(!MakeRGBA())
		{
				Log("AHTUNG", "Can't load texture.");
				return false;
		}
		if (!MakeTexture())
		{
			Log("AHTUNG", "Can't load texture in videoadapter.");
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
		Log("AHTUNG", "Trying render, while texture has not been loaded.");
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

bool CGLImageData::BeginDraw()
{
	if (isDrawing)
	{
		Log("AHTUNG", "Illegal BeginDraw() method call. EndDraw() should be called before.");
		return false;
	}
	if (TexID == 0)
	{
		Log("AHTUNG", "Trying render, while texture has not been loaded.");
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
		Log("AHTUNG", "Illegal PushQuad method call. BeginDraw() should be called before.");
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
		Log("AHTUNG", "Illegal PushQuadEx method call. BeginDraw() should be called before.");
		return false;
	}

	glTexCoord2f((float)s0/width, (float)(height-t0)/height); 
	glVertex3f(x0, ScreenH()-y0, z0);


	glTexCoord2f((float)(s1 + s0)/width, (float)(height-t0)/height); 
	glVertex3f(x0+_width,  ScreenH()-y0, z0);


	glTexCoord2f((float)(s1 + s0)/width, (float)(height-t1-t0)/height); 
	glVertex3f(x0+_width,  ScreenH()-(y0+_height), z0);


	glTexCoord2f((float)s0/width, (float)(height-t1-t0)/height); 
	glVertex3f(x0,  ScreenH()-(y0+_height), z0);


	return true;
}

bool CGLImageData::EndDraw()
{
	if (!isDrawing)
	{
		Log("AHTUNG", "Illegal EndDraw() method call. BeginDraw() should be called before.");
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
		Log("WARNING", "GLImageData. Trying to access TexID but it is 0");
	return TexID;
}
//-------------------------------------------//
//				CSprite functions			 //
//-------------------------------------------//
CSprite::CSprite()
{
	m_bFirstRendering = true;
	color = glRGBAub(255, 255, 255, 255);
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
	caption = NULL;
}
CGLWindow::~CGLWindow(){}

bool CGLWindow::gCreateWindow(int _width, int _height, byte _bpp, char* _caption)
{
	if( SDL_Init(SDL_INIT_VIDEO ) < 0 )
	{
		Log("AHTUNG", "Video initialization failed: %s\n", SDL_GetError());
		Log("AHTUNG", "Last AHTUNG was critical. Now exiting...");
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
		Log("AHTUNG", "Aaaa! SDL AHTUNG: %s", SDL_GetError());
		Log("AHTUNG", "Last AHTUNG was critical. Now exiting...");
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
		Log("AHTUNG", "Setting video mode failed: %s\n", SDL_GetError());
		Log("AHTUNG", "Last AHTUNG was critical. Now exiting...");
		SDLGLExit(1);		
	}

	SDL_Event resizeEvent;
	resizeEvent.type=SDL_VIDEORESIZE;
	resizeEvent.resize.w=width;
	resizeEvent.resize.h=height;
	SDL_PushEvent(&resizeEvent);
	SetScreenParams(width, height);
	
	glInit(width, height);
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
	SetScreenParams(width, height);
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
	FontImageName = NULL;
	base = 0;
	dist = CFONT_DEFAULT_DISTANCE;
}

CFont::~CFont()
{
	if (base != 0)
		glDeleteLists(base, 256);
	if (FontImageName != NULL)
		delete [] FontImageName;
}


bool CFont::LoadFromFile(char* filename)
{
	if (filename == NULL)
		return false;
	CFile			file;
	CGLImageData	*image;
	if (!file.Open(filename, CFILE_READ))
	{
		Log("AHTUNG","Can't Load Font %s: file  couldn't be opened.", name.data()); //TODO: filename wrte too.
		return false;
	}
	
	file.ReadLine(FontImageName);	
	CTextureManager *TexMan = CTextureManager ::Instance();
	(dynamic_cast<CTexture*>(TexMan->GetObject(FontImageName)))->Load();
	image = dynamic_cast<CTexture*>((TexMan->GetObject(FontImageName)));
	TexMan->FreeInst();
	//image->LoadTexture(FontImageName);
	font = image->GetTexID();
	file.Read(bbox, sizeof(bbox));
	file.Close();
	base = glGenLists(256);
	glBindTexture(GL_TEXTURE_2D, font);
	for(int i=0;i<256;i++)
	{
		glNewList(base+i, GL_COMPILE);
		glBegin(GL_QUADS);		
			glTexCoord2f( (float)bbox[i].x0/image->width, (float)bbox[i].y0/image->height);
			glVertex2i(0, 0);

			glTexCoord2f( (float)bbox[i].x1/image->width, (float)bbox[i].y0/image->height);
			glVertex2i(bbox[i].x1 - bbox[i].x0, 0);

			glTexCoord2f( (float)bbox[i].x1/image->width, (float)bbox[i].y1/image->height);
			glVertex2i(bbox[i].x1 - bbox[i].x0, bbox[i].y1 - bbox[i].y0);

			glTexCoord2f( (float)bbox[i].x0/image->width, (float)bbox[i].y1/image->height);
			glVertex2i(0, bbox[i].y1 - bbox[i].y0);
		glEnd();
		glTranslated(bbox[i].x1 - bbox[i].x0 + dist, 0, 0);
		glEndList();
		width[i] = (bbox[i].x1 - bbox[i].x0);
		height[i] = (bbox[i].y1 - bbox[i].y0);
	}
	return true;
}

bool CFont::SaveToFile(char * filename)
{
	if (filename == NULL)
		return false;
	CFile			file;
	file.Open(filename, CFILE_WRITE);
	file.Write(FontImageName, (DWORD)strlen(FontImageName));
	file.WriteByte((byte)0x00);
	file.Write(bbox, sizeof(bbox));
	file.Close();
	return true;
}

void CFont::Print(int x, int y, float depth, char *text)
{
	if (text == NULL)
		return;
	glPushAttrib(GL_TEXTURE_2D);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font);
	glTranslatef(x, y, depth);
	glListBase(base-32);
	glCallLists((GLsizei)strlen(text), GL_BYTE, text);
	glPopAttrib();
	glPopMatrix();
}

void CFont::PrintEx(int x, int y, float depth, char *text, ...)
{
	if (text == NULL)
		return;
	char	temp[256];
	va_list	ap;
	va_start(ap, text);
	vsprintf(temp, text, ap);
	va_end(ap);

	glPushMatrix();
	glPushAttrib(GL_LIST_BIT | GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, font);
	glTranslatef(x, y, depth);

	glListBase(base - 32);
	glCallLists((GLsizei)strlen(temp), GL_UNSIGNED_BYTE, temp);

	glPopAttrib();
	glPopMatrix();
}

void CFont::PrintRect(int x, int y, float depth, int width, int height, int offset, char *text)
{
	if (text == NULL)
		return;
	glPushAttrib(GL_SCISSOR_TEST);
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, width, height);
	Print(x, y, depth, text+offset);
	glDisable(GL_SCISSOR_TEST);
	glPopAttrib();
}

void	CFont::PrintRectEx(int x, int y, float depth, int width, int height, int offset, byte align, char *text)
{
	if (text == NULL)
		return;
	if ((uInt)offset >= strlen(text) || offset < 0)
		return;
	int xpos, ypos, swidth, sheight;
	byte tempV = align & CFONT_VALIGN_MASK, tempH = align & CFONT_HALIGN_MASK;
	swidth = GetStringWidth(text + offset);
	sheight = GetStringHeight(text + offset);
	
	if (tempV == CFONT_VALIGN_TOP)
	{
		ypos = y + height - sheight;
	}
	if (tempV == CFONT_VALIGN_BOTTOM)
	{
		ypos = y;
	}
	if (tempV == CFONT_VALIGN_CENTER)
	{
		ypos = y + height/2 - sheight/2;
	}

	if (tempH == CFONT_HALIGN_LEFT)
	{
		xpos = x;
	}
	if (tempH == CFONT_HALIGN_RIGHT)
	{
		xpos = x + width - swidth;
	}

	if (tempH == CFONT_HALIGN_CENTER)
	{
		xpos = x + width/2 - swidth/2;
	}

	glPushAttrib(GL_SCISSOR_TEST);
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, width, height);
	Print(xpos, ypos, depth, text+offset);
	glDisable(GL_SCISSOR_TEST);
	glPopAttrib();
}

void CFont::PrintSelRect(int x, int y, float depth, int width, int height, int offset, byte align, char *text, int s1, int s2)
{
	if (text == NULL)
		return;
	if ((uInt)offset >= strlen(text) || offset < 0)
		return;
	/*if (s1 > s2)
		swap(s1, s2);*/
	if (s1 > s2+1)
	{
		swap(s1, s2);
		s1++;
		s2--;
		
	}
	int xpos, ypos, swidth, sheight;
	byte tempV = align & CFONT_VALIGN_MASK, tempH = align & CFONT_HALIGN_MASK;
	swidth = GetStringWidth(text + offset);
	sheight = GetStringHeight(text + offset);
	
	if (tempV == CFONT_VALIGN_TOP)
	{
		ypos = y + height - sheight;
	}
	if (tempV == CFONT_VALIGN_BOTTOM)
	{
		ypos = y;
	}
	if (tempV == CFONT_VALIGN_CENTER)
	{
		ypos = y + height/2 - sheight/2;
	}

	if (tempH == CFONT_HALIGN_LEFT)
	{
		xpos = x;
	}
	if (tempH == CFONT_HALIGN_RIGHT)
	{
		xpos = x + width - swidth;
	}

	if (tempH == CFONT_HALIGN_CENTER)
	{
		xpos = x + width/2 - swidth/2;
	}

	glPushAttrib(GL_SCISSOR_TEST);
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, width, height);
	int selx = xpos + GetStringWidthEx(0, s1-1-offset, text+offset);
	if (!(s1 > s2 || s1 < 0 || (uInt)s2 >= strlen(text)))
	{
		int selw = GetStringWidthEx(max(s1, offset), s2, text);
		CPrimitiveRender pr;
		pr.sClr = &RGBAf(10, 50, 200, 150);
		pr.depth = depth;
		pr.grRectS(Vector2(selx, ypos), selw, sheight);
	}
	CPrimitiveRender pr;
	pr.depth = depth;
	pr.grRectS(Vector2(selx - 1, ypos), 1, min(sheight, GetStringHeight("!\0"))); //  glRGBAub(10, 10, 10, 200)
	pr.grRectS(Vector2(selx - 3, ypos), 5, 1); // glRGBAub(10, 10, 10, 200)
//	gSolidRectEx(selx - 2, ypos, 5, 1, depth, &glRGBAub(10, 10, 10, 200));
	pr.grRectS(Vector2(selx - 3, ypos + min(sheight, GetStringHeight("!\0"))), 5, 1); // glRGBAub(10, 10, 10, 200)
	Print(xpos, ypos, depth, text+offset);
	glDisable(GL_SCISSOR_TEST);
	glPopAttrib();
}

int	CFont::GetStringWidth(char *text)
{
	if (text == NULL)
		return -1;
	int r = 0, l = (int)strlen(text);
	for (int i=0;i<l;i++)
		r += width[text[i]-32]+dist;
	return r;
}

int CFont::GetStringWidthEx(int t1, int t2, char *text)
{
	if (text == NULL)
		return -1;
	if (t1 > t2 || t1 < 0)
		return -1;
	if ((uInt)t2 >= strlen(text))
		t2 = strlen(text) - 1;
	int r = 0;
	for (int i=t1;i<=t2;i++)
		r += width[text[i]-32]+dist;
	return r;
}

int	CFont::GetStringHeight(char *text)
{
	if (text == NULL)
		return -1;
	int r = 0, l = (uInt)strlen(text);
	for (int i=0;i<l;i++)
		r = Max(height[text[i]-32], r);
	return r;
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
	CRenderObject *data = dynamic_cast<CRenderObject*>(Next());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	Camera.Update();
	
	
	while (data)
	{
		if (data->visible)
		{
			glLoadIdentity();
				Camera.gTranslate();
			glTranslatef((int)data->x,(int) data->y, data->z);
			data->Render();
		}
		data = dynamic_cast<CRenderObject*>(Next());
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
	return (dynamic_cast<CRenderObject*>(a))->z <= (dynamic_cast<CRenderObject*>(b)->z);
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
unsigned int g_seed = 152406923;

int Random_Int(int min, int max)
{
	g_seed = 214013*g_seed + 2531011;
	return min + (g_seed^g_seed>>15) % (max - min + 1);
}

float Random_Float(float min, float max)
{
	g_seed=214013*g_seed+2531011;
	//return min+g_seed*(1.0f/4294967295.0f)*(max-min);
	return min+(g_seed>>16)*(1.0f/65535.0f)*(max-min);
}

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

void gDrawBBox( CBBox box )
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
	memset(&info, 0, sizeof(info));

	this->info.ParticlesActive = 0;
	this->info.MaxParticles = 10000;
	this->particles = new CParticle [info.MaxParticles];
	info.emission = 500;
	info.age = 0;
	info.sizevar = 1;
	info.plifevar = 1;
	info.life = -1;
	info.plife = 1;

	user_create = false;
	user_update = false;

	TexID = 0;

	info.sc = RGBAf(1.0,0.0,0.0,0.5);
	info.ec = RGBAf(0.0,0.0,1.0,0.5);
	info.vc = RGBAf(0.0f,0.0f,0.0f,0.0f);
}

bool CParticleSystem::Update(scalar dt)
{
	#ifdef _DEBUG_DISABLE_PARTICLES_UPDATE
		return false;
	#endif
	// Here integrating and updating values of active particles
	for(int i=0; i < info.ParticlesActive; i++)
	{
		if (!user_update)
		{
			particles[i].age += dt;
			if (particles[i].age >= particles[i].life
				)
			{
				_swap(i, info.ParticlesActive-1);
				info.ParticlesActive--;
				i--;
				continue;
			}
			particles[i].p += particles[i].v*dt;
			particles[i].c += (particles[i].dc*dt);
			particles[i].size += particles[i].dsize;

			if (info.isSnow)
				particles[i].p.x += cos(particles[i].Period)*1;

			particles[i].Period += 0.001f;
			if (particles[i].Period>PI*2) particles[i].Period = 0.0f;
		}
		else
		{
			procUserUpdate(&(particles[i]), dt);
		}
	}


	float np = info.emission * dt;	// + uncreated;

	if (info.life != -1)
	{
		info.age += dt;
		if (info.age >= info.life)
			return true;
	}

	int t = info.ParticlesActive;
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
	if (TexID == 0)
	{
		glPushAttrib(GL_TEXTURE_BIT | GL_POINTS);
		glEnable(GL_POINTS);
		glDisable(GL_TEXTURE_2D);
		glPointSize(particles[0].size);
		glBegin(GL_POINTS);
		for(int i=0;i<info.ParticlesActive;i++)
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
		

		glBindTexture(GL_TEXTURE_2D, TexID);
		
		glBegin(GL_QUADS);
		for(int i=0;i<info.ParticlesActive;i++)
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
	file.Write(&info, sizeof(CPsysteminfo));
	file.Close();
	return true;
}

bool CParticleSystem::LoadFromFile()
{
	CFile file;
	file.Open(filename.c_str(), CFILE_READ);
	file.Read(&info, sizeof(CPsysteminfo));
	file.Close();
	return true;
}

void CParticleSystem::SetGeometry( Vector2 * points, int numPoints )
{
	info.geom = points;
	info.GeomNumPoints = numPoints;
}

CParticle	*CParticleSystem::CreateParticle()
{
	int t = info.ParticlesActive;
	int i = t;
	{
		if (i >= info.MaxParticles)
			return NULL;
		if (info.ParticlesActive >= info.MaxParticles)
			return NULL;
		info.ParticlesActive++;
		
		if (!user_create)
		{
			particles[i].life  = Random_Float(info.plife ,info.plife + info.plifevar);
			if (particles[i].life < 0 )
				particles[i].life = info.plife;
			particles[i].age = 0;

			if (info.GeomNumPoints == 1)
			{
				particles[i].p = info.geom[0];
				particles[i].v = Vector2( (rand()%1000 - rand()%1000) / 1.0f, (rand()%1000 - rand()%1000) / 1.0f); 
			}
			else
				if (info.geom != NULL)
				{
					int sr = Random_Int(0, info.GeomNumPoints-2);
					particles[i].p = info.geom[sr] + (info.geom[sr+1]-info.geom[sr])*Random_Float(0.0f, 1.0f);
					particles[i].v = ((info.geom[sr]-info.geom[sr+1]).GetPerp().Normalized())/0.01f; 
				}
				else
				{
					particles[i].p = info.p;
					particles[i].v = Vector2( (rand()%1000 - rand()%1000) / 1.0f, (rand()%1000 - rand()%1000) / 1.0f); 
				}
				particles[i].Period = Random_Float(0.0f, 2*PI);

				particles[i].size = Random_Int(info.startsize, info.startsize + info.sizevar);
				particles[i].dsize = (info.endsize - info.endsize) / particles[i].life;

				particles[i].c.a = Random_Float(info.sc.a ,info.sc.a + info.vc.a);
				particles[i].c.r = Random_Float(info.sc.r ,info.sc.r + info.vc.r);
				particles[i].c.g = Random_Float(info.sc.g ,info.sc.g + info.vc.g);
				particles[i].c.b = Random_Float(info.sc.b ,info.sc.b + info.vc.b);

				particles[i].dc = (info.ec - particles[i].c)/particles[i].life;
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


CFont* CFontManager::GetFont( char* fontname )	
{
	return (CFont*)GetObject(fontname);
}

CFont* CFontManager::GetFontEx( string fontname )
{
	return (CFont*)GetObject(fontname);
}

bool CFontManager::SetCurrentFont( char* fontname )
{
	return !!(CurrentFont = GetFont(fontname));
}

bool CFontManager::Print( int x, int y, float depth, char* text, ... )
{
	if (!CurrentFont)
		return false;

	CurrentFont->PrintEx(x, y, depth, text);
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

	CurrentFont->PrintEx(x, y, depth, temp);
	return true;
}

bool CFontManager::AddObject( CObject *object )
{
	CObjectList::AddObject(object);
	if (CurrentFont == NULL)
		CurrentFont = (CFont*)object;
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
	tmp = dynamic_cast<CTexture*>(GetObject(TextureName));
	if (tmp == NULL)
	{
		Log("WARNING", "Can't load find texture with name %s", TextureName);
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
			Log("WARNING", "Can't find some texture");
			return false;
		}
		tmp->Load();
		tmp = NULL;
	}
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
		Log("WARNING", "CTextuere named %s. Trying to access TexID but it is 0", name.c_str());
		if (!loaded)
		{
			Load();
		}		
	}
	return TexID;
}

void CCamera::Assign(scalar *x, scalar *y)
{
	GetScreenParams(w, h);
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

void CPrimitiveRender::grLine(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	glColor4fv(&(lClr->r));
	glBegin(GL_LINES);
		glVertex2fv(&(v0.x));
		glVertex2fv(&(v1.x));
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grSegment(const Vector2 &v0, const Vector2 &v1)
{
	BeforeRndr();
	glColor4fv(&(lClr->r));
	glBegin(GL_LINES);
		glVertex2fv(&(v0.x));
		glVertex2fv(&(v1.x));
	glEnd();
	if (pClr)
		glColor4fv(&(pClr->r));
	glBegin(GL_POINTS);
		glVertex2fv(&(v0.x));
		glVertex2fv(&(v1.x));
	glEnd();
	AfterRndr();
}

void CPrimitiveRender::grSegmentC( const Vector2 &v0, const Vector2 &v1 )
{
	BeforeRndr();
	AfterRndr();
}


void CPrimitiveRender::grCircleL(const Vector2 &p, scalar Radius)
{
	if (!glIsList(glListCircleL))
		return;
	glPushMatrix();
	glPushAttrib(GL_TEXTURE_BIT || GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	glColor4fv(&(lClr->r));
	glTranslatef(p.x, p.y, 0.0f);
	glScalef(Radius, Radius, 0.0f);
	glCallList(glListCircleL);
	glPopAttrib();
	glPopMatrix();
}

void CPrimitiveRender::grCircleS(const Vector2 &p, scalar Radius)
{

}

void CPrimitiveRender::grCircleC(const Vector2 &p, scalar Radius)
{

}
void CPrimitiveRender::grRectL(const Vector2 &p, scalar width, scalar height)
{
	BeforeRndr();
	glColor4fv(&(lClr->r));
	glBegin(GL_LINE_LOOP);
	glVertex2f(p.x,			p.y);
	glVertex2f(p.x + width, p.y);
	glVertex2f(p.x + width, p.y + height);
	glVertex2f(p.x,			p.y + height);
	//glVertex2f(x, y);
	glEnd();
	AfterRndr();
}
void CPrimitiveRender::grRectS(const Vector2 &p, scalar width, scalar height)
{
	glPushMatrix();
	glPushAttrib(GL_TEXTURE_BIT || GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	if (sClr)
		glColor4fv(&(sClr->r));
	glBegin(GL_QUADS);
		glVertex2f(p.x, p.y);
		glVertex2f(p.x + width, p.y);
		glVertex2f(p.x + width, p.y + height);
		glVertex2f(p.x, p.y + height);
	glEnd();
	glPopAttrib();
	glPopMatrix();
}


void CPrimitiveRender::gRenderArrow(const Vector2& v0, const Vector2& v1)
{
	float fAngle = atan2(v1.y, v1.x);
	glPushAttrib(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(v0.x, v0.y, 0.0f);
	glRotatef(RadToDeg(fAngle), 0.0f, 0.0f, 1.0f);
	//glScalef(length, length, 1.0f);

	// color set

	glBegin(GL_LINES);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(0.75f, 0.2f);
		glVertex2f(1.0f, 0.0f);
		glVertex2f(0.75f,-0.2f);
	glEnd();

	glPopMatrix();
	glPopAttrib();
}

void CPrimitiveRender::grPolyC(const Vector2 &p, scalar angle, CPolygon *poly)
{
	glPushMatrix();
	glPushAttrib(GL_TEXTURE_BIT | GL_DEPTH_TEST | GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
#ifdef G_PRIM_BLEND_OPT
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDisable(GL_DEPTH_TEST);
#else
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
	glTranslatef(p.x, p.y, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, -1.0f);
	glColor4fv(&(sClr->r));

#ifdef G_POLY_TEXTURE_ENABLE
	glEnable(GL_TEXTURE_2D);
	CTextureManager *Tman = CTextureManager::Instance();
	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObject("cells"));
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
	glColor4fv(&(lClr->r));
	glBegin(GL_LINE_LOOP);
	for(int i = 0; i < poly->numV; i++)
	{
		glVertex2fv(&(poly->V[i].x));
	}
	glEnd();

#endif 
	glPopAttrib();
	glPopMatrix();
}

void CPrimitiveRender::grRingS(const Vector2 &p, scalar Radius)
{
	static int glList = -1;
	if (!glIsList(glList))
	{
		glList = glGenLists(1);

		glNewList(glList, GL_COMPILE_AND_EXECUTE);

		glBegin(GL_TRIANGLE_STRIP);

		for(int i = 0; i < 64 + 1; i ++)
		{
			Vector2 P(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
			glVertex2f(P.x, P.y);
			P = Vector2(cos(PI * (i / 32.0f)), sin(PI * (i / 32.0f)));
			glVertex2f(P.x*0.7f, P.y*0.7f); // TODO! MAGIC NUMBERS
		}

		glEnd();

		glEndList();
	}

	glPopAttrib();
	glPopMatrix();
	glPushMatrix();
	glPushAttrib(GL_TEXTURE_BIT | GL_DEPTH_TEST | GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
#ifdef G_PRIM_BLEND_OPT
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDisable(GL_DEPTH_TEST);
#else
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
	glTranslatef(p.x, p.y, 0.0f);
	// 	glColor4fv(&(col.r));

#ifdef G_POLY_TEXTURE_ENABLE
	// 	glEnable(GL_TEXTURE_2D);
	// 	CTextureManager *Tman = CTextureManager::Instance();
	// 	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObject("cells"));
	// 	Tman->FreeInst();
	// 	cells->Bind();
#endif 

	// 	glBegin(GL_TRIANGLE_FAN);
	// 	for(int i = 0; i < poly->numV; i++)
	// 	{
	// #ifdef G_POLY_TEXTURE_ENABLE
	// 		glTexCoord2f(poly->V[i].x/G_POLY_TEX_CELL_SIZE, poly->V[i].y/G_POLY_TEX_CELL_SIZE);
	// #endif 
	// 		glVertex2fv(&(poly->V[i].x));
	// 	}
	// 	glEnd();
	glScalef(Radius, Radius, 0.0f);
	glCallList(glList);


	// #ifdef G_POLY_OUTLINE_ENABLE
	// 
	// 	glEnable(GL_LINE_WIDTH);
	// 	glLineWidth(1.0f);
	// 	glColor4fv(&(lcol.r));
	// 	glBegin(GL_LINE_LOOP);
	// 	for(int i = 0; i < poly.numV; i++)
	// 	{
	// 		glVertex2fv(&(poly.V[i].x));
	// 	}
	// 	glEnd();
	// 
	// #endif 
	glPopAttrib();
	glPopMatrix();
	grCircleL(p, Radius*0.3);
}

void CPrimitiveRender::gRenderArrowEx(const Vector2 &v0,const Vector2 &v1)
{
	grRingS(v0, 20);
	gRenderArrow(v0, v1);
}

void CPrimitiveRender::BeforeRndr()
{
	glPushMatrix();
	glPushAttrib(GL_TEXTURE_BIT | GL_DEPTH_TEST | GL_BLEND | GL_LINE_WIDTH | GL_POINT_SIZE);
	glEnable(GL_POINT_SIZE);
	glLineWidth(lwidth);
	glPointSize(psize);
	glEnable(GL_LINE_WIDTH);
	glDisable(GL_TEXTURE_2D);
}

void CPrimitiveRender::AfterRndr()
{
	glPopAttrib();
	glPopMatrix();
}

void CPrimitiveRender::CheckBlend()
{
	if (BlendingOption == PRM_RNDR_OPT_BLEND_ONE)
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);		
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void CPrimitiveRender::CheckTexture()
{
	glEnable(GL_TEXTURE_2D);
	CTextureManager *Tman = CTextureManager::Instance();
	CTexture *cells = dynamic_cast<CTexture*>(Tman->GetObject("cells"));
	Tman->FreeInst();
	cells->Bind();
}

#ifdef WIN32

#include <windows.h>

PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

void setVSync(int interval)
{
	const char *extensions = (const char *)glGetString( GL_EXTENSIONS );

	if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
		return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

		if( wglSwapIntervalEXT )
			wglSwapIntervalEXT(interval);
	}
}

#endif

