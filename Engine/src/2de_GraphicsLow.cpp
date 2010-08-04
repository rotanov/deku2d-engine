#include "2de_GraphicsLow.h"

#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
//RenderObject

CRenderObject::CRenderObject() : Angle(0.0f), aabb(0, 0, 0, 0), Depth(0.0f), Position(V2_ZERO),
	Scaling(1.0f), Color(COLOR_WHITE), Visible(true), doIgnoreCamera(false)
{
	SetName("CRenderObject");
	CRenderManager::Instance()->Add(this);
};

CRenderObject::~CRenderObject()
{
	CRenderManager::Instance()->Remove(GetID());
}

void CRenderObject::SetAngle(float AAngle /*= 0.0f*/)
{
	Angle = Clamp(AAngle, 0.0f, 360.0f);
}

float CRenderObject::GetAngle() const
{
	return Angle;
}

void CRenderObject::SetLayer(size_t Layer)
{
	Depth = Layer == 0 ? 0.0f : Layer / 100.0f;
}

float CRenderObject::GetDepth() const
{
	return Depth;
}

const CAABB& CRenderObject::GetBox() const
{
	return aabb;
}

void CRenderObject::SetBox(const CAABB &box)
{
	aabb = box;
}

float CRenderObject::GetScaling() const
{
	return Scaling;
}

void CRenderObject::SetScaling(float AScaling)
{
	Scaling = AScaling;
	aabb.vMin *= Scaling;
	aabb.vMax *= Scaling;
}
//////////////////////////////////////////////////////////////////////////
//CGLImagedata

CGLImageData::CGLImageData()
{
	TexID = Width = Height = BPP = 0;
	//doCleanData = true;
	Data = NULL;
}

CGLImageData::~CGLImageData()
{
	if(glIsTexture(TexID))
		glDeleteTextures(1, &TexID);
}

bool CGLImageData::MakeTexture()
{
	if (Data == NULL) 
		return false;
	if ((Width&(Width-1)) != 0)		//	Тут мы просто выходим, если ширина или высота  не является степенью двойки.
		return false;				//	Ultimate - это использовать NOT_POWER_OF_TWO екстеншон, если он доступен;
	if ((Height&(Height-1)) != 0)	//	Иначе - дописывать в память кусок прозрачного цвета, по размеру такой, чтобы
		return false;				//	Ширина и выстоа стали ближайшими степенями двойки. Но это потом. И это @todo.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	int MODE = (BPP == 3) ? GL_RGB : GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, BPP, Width, Height, 0, MODE, GL_UNSIGNED_BYTE, Data);
	return true;
}

bool CGLImageData::LoadTexture(const string &Filename)
{
	if (!LoadFromFile(Filename))
	{
		Log("ERROR", "Can't load image->");
		return false;
	}
	if (!MakeTexture())
	{
		Log("ERROR", "Can't load texture in video memory.");
		return false;
	}
	if (doCleanData)
	{
		delete [] Data;
		Data = NULL;
	}
	return true;
}

bool CGLImageData::LoadTexture(size_t AWidth, size_t AHeight, const byte* Address)
{
	doCleanData = false;
	Data = const_cast<byte *>(Address);
	Width = AWidth;
	Height = AHeight;
	BPP = 4;
	if (!MakeTexture())
	{
		Log("ERROR", "Can't load texture in video memory.");
		return false;
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
	width = 640;	//@todo!!! Default magic numbers - delete them!
	height = 480;
	fullscreen = false;
	bpp = 32;
	caption = "Warning: CGLWindow class instance have not been initialized properly"; // assigning string constant to unallocated pointer
}

bool CGLWindow::gCreateWindow(int _width, int _height, byte _bpp, char* _caption)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
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
		caption = new char [strlen(_caption)]; // possibly leak
		strcpy(caption, _caption);
	}
	SDL_WM_SetCaption(caption, NULL);
	const SDL_VideoInfo *info = NULL;

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

	SDL_Surface *screen = SDL_SetVideoMode(width, height, bpp, flags);
	if (screen == NULL)
	{
		Log("ERROR", "Setting video mode failed: %s\n", SDL_GetError());
		Log("ERROR", "Last WARNING was critical. Now exiting...");
		SDLGLExit(1);		
	}

	SDL_Event resizeEvent;
	resizeEvent.type = SDL_VIDEORESIZE;
	resizeEvent.resize.w = width;
	resizeEvent.resize.h = height;
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
	setVSync(1);
}

//-------------------------------------------//
//				Font stuff					 //
//-------------------------------------------//
CFont::CFont()
{
	base = 0;
	Distance = CFONT_DEFAULT_DISTANCE;	// Нет! Грузить её из файла!!! Ну, по крайне мере по умолчанию ставить из файла значение.
										// Пользователь потом сам попроавит, если надо.
	CFontManager::Instance()->Add(this);
}

CFont::~CFont()
{
	if (!base)
		glDeleteLists(base, 256);
	CFontManager::Instance()->Remove(GetID());
}


bool CFont::LoadFromFile()
{
	if (Filename == "")
		return false;
	CFile			file;
	if (!file.Open(Filename, CFile::OPEN_MODE_READ))
	{
		Log("ERROR", "Can't Load Font %s: file  couldn't be opened.", GetName());
		return false;
	}
	string FontImageName;
	file.ReadString(FontImageName);	

	CTextureManager *TexMan = CTextureManager::Instance();
	Texture = TexMan->Get(FontImageName);

	Texture->CheckLoad(); // я не помню, зачем я это сюда добавил, но у меня чёто падало без этого, хотя может и не из-за этого...

	file.Read(bbox, sizeof(bbox));
	file.Close();
	base = glGenLists(256);
	Texture->Bind();
	float TmpOOWdth = 1.0f/Texture->Width, TmpOOHght = 1.0f/Texture->Height;
	for(int i=0;i<256;i++)
	{
		if (bbox[i].x0 > bbox[i].x1)
			swap(bbox[i].x0, bbox[i].x1);
		if (bbox[i].y0 > bbox[i].y1)
			swap(bbox[i].y0, bbox[i].y1);
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

bool CFont::LoadFromMemory(const byte* Address)
{
	assert(Address != NULL);
	while (*Address)
		Address++;
	Address++;

	Texture = CTextureManager::Instance()->Get("DefaultFontTexture");

	//Texture->CheckLoad(); // я не помню, зачем я это сюда добавил, но у меня чёто падало без этого, хотя может и не из-за этого...

	//file.Read(bbox, sizeof(bbox));
	memcpy(bbox, Address, sizeof(bbox));
	//file.Close();
	base = glGenLists(256);
	Texture->Bind();
	float TmpOOWdth = 1.0f/Texture->Width, TmpOOHght = 1.0f/Texture->Height;
	for(int i=0;i<256;i++)
	{
		if (bbox[i].x0 > bbox[i].x1)
			swap(bbox[i].x0, bbox[i].x1);
		if (bbox[i].y0 > bbox[i].y1)
			swap(bbox[i].y0, bbox[i].y1);
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
	if (Filename == "")
		return false;
	if (!Texture)
		return false;
	CFile file;
	file.Open(Filename, CFile::OPEN_MODE_WRITE);
	file.Write(Texture->GetName().c_str(), Texture->GetName().length());
	file.WriteByte((byte)0x00);
	file.Write(bbox, sizeof(bbox));
	file.Close();
	return true;
}

int CFont::GetStringWidth(const string &text)
{
	int r = 0;
	for (int i = 0; i < text.length(); i++)
		r += width[(byte)text[i] - 32] + Distance;
	return r;
}

int CFont::GetStringWidthEx(int t1, int t2, const string &text)
{
	if (t1 > t2 || t2 < 0)
		return 0;
	if ((unsigned int)t2 >= text.length())
		return 0;

	int r = 0;
	for (unsigned int i = t1; i <= t2; i++)
		r += width[(byte)text[i]-32] + Distance;
	return r;
}

int CFont::GetStringHeight(const string &text)
{
	int r = 0;
	for (int i = 0; i < text.length(); i++)
		r = std::max(height[(byte)text[i] - 32], r);
	return r;
}

int CFont::GetStringHeightEx(int t1, int t2, const string &text)
{
	if (t1 > t2 || t2 < 0)
		return 0;
	if ((unsigned int)t2 >= text.length())
		return 0;
	int r = 0, l = text.length();
	for (unsigned int i = 0; i < text.length(); i++)
		r = std::max(height[(byte)text[i] - 32], r);
	return r;
}

int CFont::StringCoordToCursorPos(const string &text, int x, int y)
{
	// @todo: move this function to CText
	if (text.length() == 0)
		return -1;
	
	Vector2 Local = Vector2(x, y);// - Pos;
	if (Local.x < 0)
		return -1;

	for (int i = 0; i < text.length(); i++)
	{
		int SubstrWidth = GetStringWidthEx(0, i, text);
		int SymbolCenterCoord = SubstrWidth - Distance - (width[(byte)text[i] - 32] / 2);
		if (Local.x < SymbolCenterCoord)
			return (i - 1);
	}
	return (text.length() - 1);
}


CTexture* CFont::GetTexture()
{
	return Texture;
}

void CFont::SetTexture(const string &TextureName)
{
	Texture = CTextureManager::Instance()->Get(TextureName);
}

CAABB CFont::GetSymbolsBBOX()
{
	CAABB Result;
	Result.vMin = Vector2(9999.0f, 9999.0f);
	Result.vMax = Vector2(0.0f, 0.0f);
	for(int i = 0; i < 256; i++)
	{
		if (bbox[i].x0 < Result.vMin.x)
			Result.vMin.x = bbox[i].x0;
		if (bbox[i].y0 < Result.vMin.y)
			Result.vMin.y = bbox[i].y0;
		if (bbox[i].x1 > Result.vMax.x)
			Result.vMax.x = bbox[i].x1;
		if (bbox[i].y1 > Result.vMax.y)
			Result.vMax.y = bbox[i].y1;		
	}
	return Result;
}
//////////////////////////////////////////////////////////////////////////
//Camera

void CCamera::Assign(float *x, float *y)
{
	CEngine* engine = CEngine::Instance();  // too mush routines // укоротить
	engine->GetState(CEngine::STATE_SCREEN_HEIGHT, &h);
	engine->GetState(CEngine::STATE_SCREEN_HEIGHT, &w);

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
	if (Abs(v.x) < 0.001f && Abs(v.y) < 0.001f)
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

void CCamera::SetWidthAndHeight(int AWidth, int AHeight)
{
	w = AWidth;
	h = AHeight;
	Point = p = v = Vector2(w / 2.0f, h / 2.0f);
}
//-------------------------------------------//
//				CRenderManager				 //
//-------------------------------------------//
CRenderManager::CRenderManager()
{
	SetName("Render Manager");
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
	ManagerContainer toDelete;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // !!! there. it shouln't be there
	Camera.Update();
	for (ManagerConstIterator i = Objects.begin(); i != Objects.end(); ++i)
	{
		CRenderObject *data = *i;
		if (data->isDestroyed())
		{
			toDelete.push_back(data);
			continue;
		}
		if (data->Visible)
		{
			glLoadIdentity();			
			if (!data->doIgnoreCamera)
				Camera.gTranslate();
			data->Color.glSet();
			glScalef(data->GetScaling(), data->GetScaling(), ROTATIONAL_AXIS_Z);
			glTranslatef(data->Position.x, data->Position.y, data->GetDepth());
			glRotatef(data->GetAngle(), 0.0f, 0.0f, 1.0f);
			data->Render();
		}
	}
	for (ManagerIterator i = toDelete.begin(); i != toDelete.end(); ++i)
	{
		Objects.remove(*i);
		CObject::DecRefCount(*i);
	}
#ifdef _DEBUG
	//	Camera.DrawDebug();
#endif 
	return true;
}

void CRenderManager::Print(const CText *Text) const
{
	//	_Print(reinterpret_cast<const byte*>(text));
	glEnable(GL_TEXTURE_2D);
	Text->GetFont()->Texture->Bind();
	glListBase(Text->GetFont()->base-32);		
	glCallLists(static_cast<GLsizei>(Text->GetText().length()), GL_UNSIGNED_BYTE, Text->GetText().c_str());
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

CFontManager::CFontManager() : DefaultFont(NULL), CurrentFont(NULL)
{
	SetName("Font manager");
}


void CFontManager::Init()
{
	CTexture* DefaultFontTexture = CFactory::Instance()->New<CTexture>("DefaultFontTexture");
	DefaultFontTexture->LoadTexture(IMAGE_DEFAULT_FONT_WIDTH, IMAGE_DEFAULT_FONT_HEIGHT, reinterpret_cast<byte *>(IMAGE_DEFAULT_FONT_DATA));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	DefaultFont = new CFont;
	DefaultFont->SetName("DefaultFont");
	DefaultFont->LoadFromMemory(reinterpret_cast<byte *>(BINARY_DATA_DEFAULT_FONT));
	CurrentFont = DefaultFont;
	assert(DefaultFont != NULL);
}


CFont* CFontManager::GetFont(const string &fontname)	
{
	CFont *TempFont = NULL;
	TempFont = Get(fontname);
	if (TempFont)
		TempFont->CheckLoad();
	return TempFont;
}

bool CFontManager::SetCurrentFont(const char* fontname)
{
	CurrentFont = GetFont(fontname);
	return !!CurrentFont;
}

bool CFontManager::AddFont(CFont *AObject)
{	
	CCommonManager <list <CFont*> >::Add(AObject);
	if (CurrentFont == NULL)
		CurrentFont = AObject;
	return true;
}

CFont* CFontManager::Font()
{
	assert(CurrentFont);
	return CurrentFont;
}
//////////////////////////////////////////////////////////////////////////
//						CTexture Manager
//////////////////////////////////////////////////////////////////////////

CTextureManager::CTextureManager()
{
	SetName("Texture manager");
}

// CTexture* CTextureManager::GetObject(const string &TextureName)
// {
// 	CTexture *TempTexture = NULL;
// 	TempTexture = dynamic_cast<CTexture*>(GetObject(&TextureName));
// 	if (TempTexture)
// 		TempTexture->CheckLoad();
// 	return TempTexture;
// }


//////////////////////////////////////////////////////////////////////////
//					CTexture
//////////////////////////////////////////////////////////////////////////
GLuint CTexture::GetTexID()
{
	if (TexID == 0)
	{
		Log("ERROR", "CTextuere named %s. Trying to access TexID but it is 0", GetName());
		if (!Loaded)
		{
			LoadFromFile();
		}		
	}
	return TexID;
}

// CTexture::CTexture(const string &AFilename) : CBaseResource::Filename(AFilename)
// {	
// 	SetName("CTexture");
// 	CTextureManager *TexMan = CTextureManager::Instance();
// 	TexMan->AddObject(this);
// }

CTexture::CTexture()
{
	SetName("CTexture");
	CTextureManager::Instance()->Add(this);
}

CTexture::~CTexture()
{
	CTextureManager::Instance()->Remove(GetID());
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
	if (Filename == "")
	{
		Log("ERROR", "Trying to load texture with name %s; But it has not been found in ResourceList(s)\n\t or Resource List Has not been loaded", GetName());
		return false;
	}
	if (!Loaded)
		if (LoadTexture(Filename))
			Loaded = true;
	return Loaded;
}


// всё что я тут понагородил мне самому не нравится... каких-то макросов понаизобретал.. зато универсально и ынтерпрайзно..

#if defined(_WIN32) || defined(__linux)        // <platforms, that support swap interval control>

#if defined(_WIN32)
#include <windows.h>
#define GL_GET_PROC_ADDRESS_ARG_TYPE (const char*)
#define SWAP_INTERVAL_EXTENSION_NAME "WGL_EXT_swap_control"
#define SWAP_INTERVAL_PROC_NAME_STR "wglSwapIntervalEXT"
#define SWAP_INTERVAL_PROC_NAME wglSwapIntervalEXT
#define GET_PROC_ADDRESS_FUNC wglGetProcAddress
#elif defined(__linux)
#include <GL/glx.h>
#define GL_GET_PROC_ADDRESS_ARG_TYPE (const GLubyte*)
#define SWAP_INTERVAL_EXTENSION_NAME "GLX_SGI_swap_control"
#define SWAP_INTERVAL_PROC_NAME_STR "glXSwapIntervalSGI"
#define SWAP_INTERVAL_PROC_NAME glXSwapIntervalSGI
#define GET_PROC_ADDRESS_FUNC glXGetProcAddress
#endif

#define GET_PROC_ADDRESS(x,y) ((x(GL_GET_PROC_ADDRESS_ARG_TYPE(y))))

SWAP_INTERVAL_PROC SWAP_INTERVAL_PROC_NAME = 0;

#endif                        // </platforms, that support swap interval control>

// да, функция будет для всех, но для неподдерживаемых она просто ничего не будет делать..
// это чтобы не городить ифдефы в другом месте..

void setVSync(int interval)
{
#if defined(_WIN32) || defined(__linux)        // <platforms, that support swap interval control>
#if defined(_WIN32)
	const char *extensions = (const char *) glGetString(GL_EXTENSIONS);
#elif defined(__linux)
	const char *extensions = (const char *) glXGetClientString(glXGetCurrentDisplay(), GLX_EXTENSIONS);
#endif

	if (strstr(extensions, SWAP_INTERVAL_EXTENSION_NAME) != 0)
	{
		SWAP_INTERVAL_PROC_NAME = (SWAP_INTERVAL_PROC) GET_PROC_ADDRESS(GET_PROC_ADDRESS_FUNC, SWAP_INTERVAL_PROC_NAME_STR);

		if (SWAP_INTERVAL_PROC_NAME)
			SWAP_INTERVAL_PROC_NAME(interval);
	}
	else
	{
		Log("ERROR", SWAP_INTERVAL_EXTENSION_NAME " is not supported on your computer.");
	}
#endif                        // </platforms, that support swap interval control>
}

//////////////////////////////////////////////////////////////////////////
// CText

CText::CText() : Text(""), Font(CFontManager::Instance()->GetDefaultFont())
{	
	assert(Font != NULL);
	doIgnoreCamera = true;
}

void CText::Render()
{
	//assert(Text != "");
	CRenderManager::Instance()->Print(this);
}

CFont* CText::GetFont() const
{
	assert(Font != NULL);
	return Font;
}

string& CText::GetText()
{
	return Text;
}

const string& CText::GetText() const
{
	return Text;
}

void CText::SetFont(CFont *AFont)
{
	assert(AFont != NULL);
	Font = AFont;
}

void CText::SetText(const string &AText)
{
	Text = AText;
}

int CText::Height()
{
	assert(Font != NULL);
	return Font->GetStringHeight(Text);
}

int CText::Width()
{
	assert(Font != NULL);
	return Font->GetStringWidth(Text);
}

CText& CText::operator=(const string &AText)
{
	Text = AText;
	return *this;
}

std::string CText::operator+(const CText &Text) const
{
	return this->GetText() + Text.GetText();
}

std::string CText::operator+(const char *Text) const
{
	return this->GetText() + Text;
}
