#include "2de_GraphicsLow.h"
#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
//RenderObject

CRenderObject::CRenderObject() : position(V2_ZERO), depth(0.0f), visible(true), color(1.0f, 1.0f, 1.0f, 1.0f)
{
	CEngine::Instance()->RenderManager.AddObject(this);
	SetName("CRenderObject");
	type |= T_RENDERABLE;
};

CRenderObject::~CRenderObject()
{
	CEngine::Instance()->RenderManager.DelObject(GetID());
}
//////////////////////////////////////////////////////////////////////////
//CGLImagedata

CGLImageData::CGLImageData()
{
	TexID = width = height = bpp = 0;
	CleanData = true;
	data = NULL;
}

CGLImageData::~CGLImageData()
{
	if(glIsTexture(TexID))
		glDeleteTextures(1, &TexID);
}

bool CGLImageData::MakeTexture()
{
	if (data == NULL) 
		return false;
	if ((width&(width-1)) != 0)		//	Тут мы просто выходим, если ширина или высота  не является степенью двойки.
		return false;				//	Ultimate - это использовать NOT_POWER_OF_TWO екстеншон, если он доступен;
	if ((height&(height-1)) != 0)	//	Иначе - дописывать в память кусок прозрачного цвета, по размеру такой, чтобы
		return false;				//	Ширина и выстоа стали ближайшими степенями двойки. Но это потом. И это TODO.
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

GLuint CGLImageData::GetTexID()
{
	if (TexID == 0)
		Log("ERROR", "GLImageData. Trying to access TexID but it is 0");
	return TexID;
}

//////////////////////////////////////////////////////////////////////////
//CGLWindow

CGLWindow::CGLWindow()
{
	SetName("GLWindow");
	width = 640;	//TODO!!! Default magic numbers - delete them!
	height = 480;
	fullscreen = false;
	bpp = 32;
	caption = "Warning: CGLWindow class instance have not been initialized properly";
}
CGLWindow::~CGLWindow()
{

}

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





	setVSync(0);
}
CGLWindow *CGLWindow::_instance = NULL;
CGLWindow* CGLWindow::Instance()
{
	if(!_instance)
	{
		_instance = new CGLWindow();
		SingletoneKiller.AddObject(_instance);
	}
	return _instance;
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
	CFontManager::Instance()->AddObject(this);
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
	if (!file.Open(filename, CFile::OPEN_MODE_READ))
	{
		Log("ERROR","Can't Load Font %s: file  couldn't be opened.", GetName()); //TODO: filename wrte too.
		return false;
	}
	char *FontImageName = NULL;
	file.ReadLine(FontImageName);	
	CTextureManager *TexMan = CTextureManager::Instance();
	Texture = TexMan->GetTextureByName(FontImageName);

	file.Read(bbox, sizeof(bbox));
	file.Close();
	base = glGenLists(256);
	Texture->Bind();
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
	file.Open(filename, CFile::OPEN_MODE_WRITE);
	file.Write(Texture->GetName(), (DWORD)strlen(Texture->GetName()));
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

CTexture* CFont::GetTexture()
{
	return Texture;
}

void CFont::SetTexture(char *TextureName)
{
	Texture = CEngine::Instance()->TextureManager->GetTextureByName(TextureName);
}
//////////////////////////////////////////////////////////////////////////
//Camera

void CCamera::Assign(scalar *x, scalar *y)
{
	CEngine* engine = CEngine::Instance();  // too mush routines // укоротить
	engine->GetState(STATE_SCREEN_HEIGHT, &h);
	engine->GetState(STATE_SCREEN_HEIGHT, &w);

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
		v = V2_ZERO;
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
///	gDrawBBox(world);
	glColor4f(0.4f, 0.8f, 0.3f, 0.5f);
///	gDrawBBox(view);
	glColor4f(0.3f, 0.4f, 0.8f, 0.5f);
//	gDrawBBox(outer);
}

void CCamera::gTranslate()
{
	glTranslatef((int) (-Point.x + w/2.0f), (int) (-Point.y + h/2.0f), 0.0f);
}

CCamera::CCamera()
{
	SetName("CCamera");
	view = world = CAABB(100, 100, 540, 380);
	outer = CAABB(-1024, 0, 2048, 512);
	Point = p = v = V2_ZERO;
	Atx = Aty = NULL;
	Assigned = false;
	SetWidthAndHeight(CGLWindow::Instance()->width, CGLWindow::Instance()->height);
}

void CCamera::SetWidthAndHeight( int AWidth, int AHeight )
{
	w = AWidth;
	h = AHeight;
	Point = p = v = Vector2(w/2.0f, h/2.0f);
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
			glTranslatef(data->position.x, data->position.y, data->depth);
			data->Render();
		}
		data = dynamic_cast<CRenderObject*>(Next());
	}
	glPopMatrix();

#ifdef _DEBUG
	//	Camera.DrawDebug();
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
//////////////////////////////////////////////////////////////////////////
//			Font Manager
//////////////////////////////////////////////////////////////////////////

CFontManager::~CFontManager()
{

}

CFontManager::CFontManager()
{
	SetName("Font manager");
	CurrentFont = NULL;
}

CFontManager* CFontManager::Instance()
{
	if (!_instance)
	{
		_instance = new CFontManager;
		SingletoneKiller.AddObject(_instance);
	}
	return _instance;
}

CFontManager* CFontManager::_instance = NULL;


CFont* CFontManager::GetFont(const char* fontname )	
{
	CFont *TempFont = NULL;
	TempFont = dynamic_cast<CFont*>(GetObject(&((string)fontname)));
	if (TempFont)
		TempFont->CheckLoad();
	return TempFont;
}

CFont* CFontManager::GetFontEx( string fontname )
{
	return dynamic_cast<CFont*>(GetObject(&fontname));
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
	if (!_instance)
	{
		_instance = new CTextureManager;
		SingletoneKiller.AddObject(_instance);
	}
	return _instance;
}


CTextureManager::CTextureManager()
{
	SetName("Texture manager");
}

CTextureManager::~CTextureManager()
{

}

CTexture* CTextureManager::GetTextureByName( const string &TextureName )
{
	CTexture *TempTexture = NULL;
	TempTexture = dynamic_cast<CTexture*>(GetObject(&TextureName));
	if (TempTexture)
		TempTexture->CheckLoad();
	return TempTexture;
}
CTextureManager* CTextureManager::_instance = 0;


//////////////////////////////////////////////////////////////////////////
//					CTexture
//////////////////////////////////////////////////////////////////////////
GLuint CTexture::GetTexID()
{
	if (TexID == 0)
	{
		Log("ERROR", "CTextuere named %s. Trying to access TexID but it is 0", GetName());
		if (!loaded)
		{
			LoadFromFile();
		}		
	}
	return TexID;
}

CTexture::CTexture(char * vfilename)
{	
	filename = vfilename;
	SetName("CTexture");
	CTextureManager *TexMan = CTextureManager::Instance();
	TexMan->AddObject(this);
}

CTexture::CTexture()
{
	SetName("CTexture");
	CTextureManager::Instance()->AddObject(this);
}

CTexture::~CTexture()
{

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
	if (filename == "")
	{
		Log("ERROR", "Trying to load texture with name %s; But it has not been found in ResourceList(s)\n\t or Resource List Has not been loaded", GetName());
		return false;
	}
	if (!loaded)
		if (LoadTexture(filename.c_str()))
			loaded = true;
	return loaded;
}

#ifdef _WIN32

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

#endif  //_WIN32

