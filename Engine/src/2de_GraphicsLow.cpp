#include "2de_GraphicsLow.h"

#include "2de_MathUtils.h"
#include "2de_Engine.h"

//////////////////////////////////////////////////////////////////////////
//Vector2, Vector4 - some graphics integrated into math
void Vector2::glTranslate() const
{
	glTranslatef(x, y, 0.0f);
}

void Vector2::glScale() const
{
	glScalef(x, y, 1.0f);
}

void Vector2::glVertex() const
{
	glVertex2f(x, y);
}

void Vector2::glTexCoord() const
{
	glTexCoord2f(x, y);
}

void Vector4::glSet() const
{
	glColor4fv(&x);
}

//////////////////////////////////////////////////////////////////////////
// CRenderableUnitInfo

CRenderableUnitInfo::CRenderableUnitInfo() : Position(V2_ZERO), Color(COLOR_WHITE), doIgnoreCamera(false), Angle(0.0f), Depth(0.0f), Scaling(1.0f)
{

}

void CRenderableUnitInfo::SetAngle(float AAngle /*= 0.0f*/)
{
	Angle = Clamp(AAngle, 0.0f, 360.0f);
}

float CRenderableUnitInfo::GetAngle() const
{
	return Angle;
}

void CRenderableUnitInfo::SetLayer(unsigned int Layer)
{
	Depth = Layer == 0 ? 0.0f : Layer / 100.0f;
}

float CRenderableUnitInfo::GetDepth() const
{
	return Depth;
}

float CRenderableUnitInfo::GetScaling() const
{
	return Scaling;
}

void CRenderableUnitInfo::SetScaling(float AScaling)
{
	Scaling = AScaling;
// 	Box.Min *= Scaling;
// 	Box.Max *= Scaling;
}

unsigned int CRenderableUnitInfo::GetLayer() const
{
	return Depth * 100.0f;
}


//////////////////////////////////////////////////////////////////////////
// CRenderable

CRenderable::CRenderable() : Box(0, 0, 0, 0), Scene(NULL), Visible(true)
{
	SetName("CRenderable");
	PutIntoScene(CSceneManager::Instance()->GetCurrentScene());
};

CRenderable::~CRenderable()
{
	CRenderManager::Instance()->Remove(this);

	Scene->RemoveRenderable(this);
}

const CBox CRenderable::GetBox() const
{
 	CBox TempBox = Box;
 	TempBox.Min *= GetScaling();
 	TempBox.Max *= GetScaling();
	//Box.RotateByAngle(Angle);
	return TempBox;
}

void CRenderable::SetBox(const CBox &ABox)
{
	Box = ABox;
}

void CRenderable::PutIntoScene(CAbstractScene *AScene)
{
	assert(AScene != NULL);
	if (Scene != NULL)
		Scene->RemoveRenderable(this);
	Scene = AScene;
	Scene->AddRenderable(this);
}

CAbstractScene* CRenderable::GetScene() const
{
	assert(Scene != NULL);
	return Scene;
}

bool CRenderable::GetVisibility() const
{
	return Visible;
}

void CRenderable::SetVisibility( bool AVisible )
{
	Visible = AVisible;
}

//////////////////////////////////////////////////////////////////////////
// CGLImagedata

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
// CGLWindow

CGLWindow::CGLWindow() : isCreated(false)
{
	SetName("GLWindow");
	Width = 640;	//@todo!!! Default magic numbers - delete them!
	Height = 480;
	Fullscreen = false;
	BPP = 32;
	Caption = "Warning: CGLWindow class instance have not been initialized properly";
}

bool CGLWindow::gCreateWindow(bool AFullscreen, int AWidth, int AHeight, byte ABPP, const string &ACaption)
{
	Fullscreen = AFullscreen;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		Log("ERROR", "Video initialization failed: %s\n", SDL_GetError());
		Log("ERROR", "Last WARNING was critical. Now exiting...");
		SDL_Quit();
	}

	Width = AWidth;
	Height = AHeight;
	BPP = ABPP;

	SetCaption(ACaption);

	const SDL_VideoInfo *info = NULL;
	info = SDL_GetVideoInfo();

	if (info == NULL)
	{
		Log("ERROR", "Aaaa! SDL WARNING: %s", SDL_GetError());
		Log("ERROR", "Last WARNING was critical. Now exiting...");
		SDL_Quit();
	}


	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,		8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,		8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,		8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,		16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,	1);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,		8);
	// 	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	// 	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);
	// 	glEnable(0x809D);

	int flags = SDL_OPENGL;
	if(Fullscreen == true)
	{
		flags |= SDL_FULLSCREEN;
	}

	SDL_Surface *screen = SDL_SetVideoMode(Width, Height, BPP, flags);
	if (screen == NULL)
	{
		Log("ERROR", "Setting video mode failed: %s\n", SDL_GetError());
		Log("ERROR", "Last WARNING was critical. Now exiting...");
		SDL_Quit();
	}

	SDL_Event resizeEvent;
	resizeEvent.type = SDL_VIDEORESIZE;
	resizeEvent.resize.w = Width;
	resizeEvent.resize.h = Height;
	SDL_PushEvent(&resizeEvent);

	glInit(Width, Height);

	CPrimitiveRender temp;
	temp.Init();
	isCreated = true;
	return true;
}

bool CGLWindow::gCreateWindow()
{
	return gCreateWindow(Fullscreen, Width, Height, BPP, Caption);
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
	//glClearColor(0.0, 0.0, 0.0, 0.5);
	SetBackgroundColor(RGBAf(0.0, 0.0, 0.0, 0.5));

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

unsigned int CGLWindow::GetWidth() const
{
	return Width;
}

unsigned int CGLWindow::GetHeight() const
{
	return Height;
}

void CGLWindow::SetWidth(unsigned int AWidth)
{
	// bad, i think.. if window's already created, we should just resize it..
	assert(!isCreated);
	Width = AWidth;
}

void CGLWindow::SetHeight(unsigned int AHeight)
{
	// same as in SetWidth
	assert(!isCreated);
	Height = AHeight;
}

string CGLWindow::GetCaption() const
{
	return Caption;
}

void CGLWindow::SetCaption(const string &ACaption)
{
	Caption = ACaption;
	SDL_WM_SetCaption(Caption.c_str(), NULL);
}

void CGLWindow::SetBackgroundColor(const RGBAf &AColor)
{
	glClearColor(AColor.x, AColor.y, AColor.z, AColor.w);
}


//////////////////////////////////////////////////////////////////////////
// CFont

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
	CFontManager::Instance()->Remove(this);
}


bool CFont::LoadFromFile()
{
	if (Filename == "")
		return false;
	CFile			file;
	if (!file.Open(Filename, CFile::OPEN_MODE_READ))
	{
		Log("ERROR", "Can't Load Font %s: file  couldn't be opened.", GetName().c_str());
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
		if (bbox[i].Min.x > bbox[i].Max.x)
			swap(bbox[i].Min.x, bbox[i].Max.x);
		if (bbox[i].Min.y > bbox[i].Max.y)
			swap(bbox[i].Min.y, bbox[i].Max.y);
		glNewList(base + i, GL_COMPILE);
			float x0 = bbox[i].Min.x, x1 = bbox[i].Max.x;
			float y0 = bbox[i].Min.y, y1 = bbox[i].Max.y;
			glBegin(GL_QUADS);		
				glTexCoord2f(x0 * TmpOOWdth, y0 * TmpOOHght);
				glVertex2i(0, 0);

				glTexCoord2f(x1 * TmpOOWdth, y0 * TmpOOHght);
				glVertex2i(x1 - x0, 0);

				glTexCoord2f(x1 * TmpOOWdth, y1 * TmpOOHght);
				glVertex2i(x1 - x0, y1 - y0);

				glTexCoord2f(x0 * TmpOOWdth, y1 * TmpOOHght);
				glVertex2i(0, y1 - y0);
			glEnd();
			glTranslated(x1 - x0 + Distance, 0, 0);
		glEndList();
		Width[i] = (x1 - x0);
		Height[i] = (y1 - y0);
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
	
	memcpy(bbox, Address, sizeof(bbox));
	base = glGenLists(256);
	Texture->Bind();
	float TmpOOWdth = 1.0f/Texture->Width, TmpOOHght = 1.0f/Texture->Height;
	for(int i=0;i<256;i++)
	{
		if (bbox[i].Min.x > bbox[i].Max.x)
			swap(bbox[i].Min.x, bbox[i].Max.x);
		if (bbox[i].Min.y > bbox[i].Max.y)
			swap(bbox[i].Min.y, bbox[i].Max.y);
		glNewList(base + i, GL_COMPILE);
		float x0 = bbox[i].Min.x, x1 = bbox[i].Max.x;
		float y0 = bbox[i].Min.y, y1 = bbox[i].Max.y;
		glBegin(GL_QUADS);		
		glTexCoord2f(x0 * TmpOOWdth, y0 * TmpOOHght);
		glVertex2i(0, 0);

		glTexCoord2f(x1 * TmpOOWdth, y0 * TmpOOHght);
		glVertex2i(x1 - x0, 0);

		glTexCoord2f(x1 * TmpOOWdth, y1 * TmpOOHght);
		glVertex2i(x1 - x0, y1 - y0);

		glTexCoord2f(x0 * TmpOOWdth, y1 * TmpOOHght);
		glVertex2i(0, y1 - y0);
		glEnd();
		glTranslated(x1 - x0 + Distance, 0, 0);
		glEndList();
		Width[i] = (x1 - x0);
		Height[i] = (y1 - y0);
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

float CFont::GetStringWidth(const string &text)
{
	float r = 0;
	for (int i = 0; i < text.length(); i++)
		r += Width[(byte)text[i] - 32] + Distance;
	return r;
}

float CFont::GetStringWidthEx(int t1, int t2, const string &text)
{
	if (t1 > t2 || t2 < 0)
		return 0;
	if ((unsigned int)t2 >= text.length())
		return 0;

	int r = 0;
	for (unsigned int i = t1; i <= t2; i++)
		r += Width[(byte)text[i]-32] + Distance;
	return r;
}

float CFont::GetStringHeight(const string &text)
{
	float r = 0;
	for (int i = 0; i < text.length(); i++)
		r = std::max(Height[(byte)text[i] - 32], r);
	return r;
}

float CFont::GetStringHeightEx(int t1, int t2, const string &text)
{
	if (t1 > t2 || t2 < 0)
		return 0;
	if ((unsigned int)t2 >= text.length())
		return 0;
	float r = 0, l = text.length();
	for (unsigned int i = 0; i < text.length(); i++)
		r = std::max(Height[(byte)text[i] - 32], r);
	return r;
}

CTexture* CFont::GetTexture()
{
	return Texture;
}

void CFont::SetTexture(const string &TextureName)
{
	Texture = CTextureManager::Instance()->Get(TextureName);
}

CBox CFont::GetSymbolsBBOX()
{
	CBox Result;
	Result.Min = Vector2(9999.0f, 9999.0f);
	Result.Max = Vector2(0.0f, 0.0f);
	for(int i = 0; i < 256; i++)
	{
		Result.Add(bbox[i].Min);
		Result.Add(bbox[i].Max);
// 		if (bbox[i].x0 < Result.Min.x)
// 			Result.Min.x = bbox[i].x0;
// 		if (bbox[i].y0 < Result.Min.y)
// 			Result.Min.y = bbox[i].y0;
// 		if (bbox[i].x1 > Result.Max.x)
// 			Result.Max.x = bbox[i].x1;
// 		if (bbox[i].y1 > Result.Max.y)
// 			Result.Max.y = bbox[i].y1;		
	}
	return Result;
}

float CFont::GetDistance() const
{
	return Distance;
}

float CFont::SymbolWidth( unsigned int Index ) const
{
	return Width[Index];
}

Vector2Array<4> CFont::GetTexCoords( unsigned int Charachter ) /*const Vector2Array<4>& GetTexCoords(unsigned int Charachter) // <-- warning: reference to local variable result returned */
{
	Vector2Array<4> result;
	result[0] = Vector2(bbox[Charachter - 32].Min.x / Texture->Width,
		bbox[Charachter - 32].Min.y / Texture->Height);
	result[1] = Vector2(bbox[Charachter - 32].Max.x / Texture->Width,
		bbox[Charachter - 32].Min.y / Texture->Height);
	result[2] = Vector2(bbox[Charachter - 32].Max.x / Texture->Width,
		bbox[Charachter - 32].Max.y / Texture->Height);
	result[3] = Vector2(bbox[Charachter - 32].Min.x / Texture->Width,
		bbox[Charachter - 32].Max.y / Texture->Height);
	return result;
}
//////////////////////////////////////////////////////////////////////////
// CCamera

void CCamera::Assign(float *x, float *y)
{
	CEngine* engine = CEngine::Instance();  // too mush routines // укоротить
	h = CGLWindow::Instance()->GetHeight();
	w = CGLWindow::Instance()->GetWidth();
	//engine->GetState(CEngine::STATE_SCREEN_HEIGHT, &h);
	//engine->GetState(CEngine::STATE_SCREEN_HEIGHT, &w); // OMG, HEIGHT to w? is it intentionally or accidentally? damn it, i don't know..

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
	view = world = CBox(100, 100, 540, 380);
	outer = CBox(-1024, 0, 2048, 512);
	Point = p = v = V2_ZERO;
	Atx = Aty = NULL;
	Assigned = false;
	SetWidthAndHeight(CGLWindow::Instance()->GetWidth(), CGLWindow::Instance()->GetHeight());
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
}

CRenderManager::~CRenderManager()
{

}

bool CRenderManager::DrawObjects()
{
	//ManagerContainer toDelete;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // !!! there. it shouln't be there
	Camera.Update();

	CRenderable *data;

	for (ManagerConstIterator i = Objects.begin(); i != Objects.end(); ++i)
	{
		data = *i;
		/*if (data->isDestroyed())
		{
			toDelete.push_back(data);
			continue;
		}*/
		if (!CSceneManager::Instance()->InScope(data->GetScene()))
			continue;
		if (data->GetVisibility())
		{
			glLoadIdentity();	
			glTranslatef(0.375, 0.375, 0.0); //accuracy tip from MSDN help
			if (!data->doIgnoreCamera)
				Camera.gTranslate();
			data->Color.glSet();
			glScalef(data->GetScaling(), data->GetScaling(), ROTATIONAL_AXIS_Z);
			glTranslatef(data->Position.x, data->Position.y, data->GetDepth());
			glRotatef(data->GetAngle(), 0.0f, 0.0f, 1.0f);
			data->Render();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	glLoadIdentity();
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	FontVertices.RenderPrimitive(GL_QUADS);
	glDisable(GL_TEXTURE_2D);
	QuadVertices.RenderPrimitive(GL_QUADS);
	LineVertices.RenderPrimitive(GL_LINES);
	//PointVertices.RenderPrimitive(GL_POINTS);
	PointVertices.Clear();
	//////////////////////////////////////////////////////////////////////////
	/*for (ManagerIterator i = toDelete.begin(); i != toDelete.end(); ++i)
	{
		Objects.remove(*i);
		CObject::DecRefCount(*i);
	}*/
#ifdef _DEBUG
	//	Camera.DrawDebug();
#endif 
	return true;
}

void CRenderManager::Print(const CText *Text)
{
 	glEnable(GL_TEXTURE_2D);
 	Text->GetFont()->Texture->Bind();
// 	glListBase(Text->GetFont()->base-32);		
// 	glCallLists(static_cast<GLsizei>(Text->GetText().length()), GL_UNSIGNED_BYTE, Text->GetText().c_str());
	float dx = 0.0f;
	for(int i = 0; i < Text->GetText().length(); i++)
	{
		float liwidth = Text->GetFont()->bbox[Text->GetText()[i] - 32].Width();
		float liheight = Text->GetFont()->bbox[Text->GetText()[i] - 32].Height();
		Vector2Array<4> TempTexCoords = Text->GetFont()->GetTexCoords(Text->GetText()[i]);
		FontVertices.PushVertex(Text, Vector2(dx,			0.0f), Text->Color, TempTexCoords[0]);
		FontVertices.PushVertex(Text, Vector2(liwidth + dx, 0.0f), Text->Color, TempTexCoords[1]);
		FontVertices.PushVertex(Text, Vector2(liwidth + dx, liheight), Text->Color, TempTexCoords[2]);
		FontVertices.PushVertex(Text, Vector2(dx,			liheight), Text->Color, TempTexCoords[3]);
		dx += Text->GetFont()->bbox[Text->GetText()[i] - 32].Width() + 1;
	}
}

void CRenderManager::DrawBox( const CGrRect *Rectangle )
{
	if (Rectangle->isLineDrawn)
	{
		LineVertices.PushVertex(Rectangle, Rectangle->Rectangle.v0);
		LineVertices.PushVertex(Rectangle, Rectangle->Rectangle.v1);
		LineVertices.PushVertex(Rectangle, Rectangle->Rectangle.v1);
		LineVertices.PushVertex(Rectangle, Rectangle->Rectangle.v2);
		LineVertices.PushVertex(Rectangle, Rectangle->Rectangle.v2);
		LineVertices.PushVertex(Rectangle, Rectangle->Rectangle.v3);
		LineVertices.PushVertex(Rectangle, Rectangle->Rectangle.v3);
		LineVertices.PushVertex(Rectangle, Rectangle->Rectangle.v0);
		return;
	}
	QuadVertices.PushVertex(Rectangle, Rectangle->Rectangle.v0);
	QuadVertices.PushVertex(Rectangle, Rectangle->Rectangle.v1);
	QuadVertices.PushVertex(Rectangle, Rectangle->Rectangle.v2);
	QuadVertices.PushVertex(Rectangle, Rectangle->Rectangle.v3);
}

void CRenderManager::DrawPoint(const CGrPoint *Point)
{
	PointVertices.PushVertex(Point, Point->Position);
}

void CRenderManager::DrawLine(const CGrLine *Line)
{
	LineVertices.PushVertex(Line, Line->Segment.v0);
	LineVertices.PushVertex(Line, Line->Segment.v1);
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

void gDrawBBox( CBox box )
{
	glPushMatrix();
	glPushAttrib(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	glEnable(GL_LINE_WIDTH);
	glLineWidth(4);
	glTranslatef(0.0f, 0.0f, 0.99f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(box.Min.x, box.Min.y);	
	glVertex2f(box.Max.x, box.Min.y);
	glVertex2f(box.Max.x, box.Max.y);
	glVertex2f(box.Min.x, box.Max.y);
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
	
	DefaultFont = CFactory::Instance()->New<CFont>("DefaultFont");
	DefaultFont->LoadFromMemory(reinterpret_cast<byte *>(BINARY_DATA_DEFAULT_FONT));

	//DefaultFont = new CFont;
	//DefaultFont->SetName("DefaultFont");
	//DefaultFont->LoadFromMemory(reinterpret_cast<byte *>(BINARY_DATA_DEFAULT_FONT));

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

CFontManager::~CFontManager()
{
	//if (DefaultFont != NULL)
		//SAFE_DELETE(DefaultFont);
	// DefaultFont is managed now..
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
		Log("ERROR", "CTextuere named %s. Trying to access TexID but it is 0", GetName().c_str());
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
	CTextureManager::Instance()->Remove(this);
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
		Log("ERROR", "Trying to load texture with name %s; But it has not been found in ResourceList(s)\n\t or Resource List Has not been loaded", GetName().c_str());
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

CText::CText(const string &AText) : Text(AText), Font(CFontManager::Instance()->GetDefaultFont())
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

float CText::StringCoordToCursorPos(int x, int y) const
{
	// @todo: move this function to CText
	if (Text.length() == 0)
		return -1;
	Vector2 Local = Vector2(x, y) - Position;
	if (Local.x < 0)
		return -1;

	for (int i = 0; i < Text.length(); i++)
	{
		float SubstrWidth = Font->GetStringWidthEx(0, i, Text);
		float SymbolCenterCoord = SubstrWidth - Font->GetDistance() - (Font->SymbolWidth((byte)Text[i] - 32) / 2.0f);
		if (Local.x < SymbolCenterCoord)
			return (i - 1);
	}
	return (Text.length() - 1.0f);
}

CText::~CText()
{
	// this is destructor. i can set breakpoint here
}


//////////////////////////////////////////////////////////////////////////
// CAbstractScene

CAbstractScene::CAbstractScene()
{
	SetName("Some abstract scene");
}

//////////////////////////////////////////////////////////////////////////
//CScene

void CScene::Render()
{
	// блеадь, дублирование кода из CRenderManager::draw
	// АХАХА, его нет.
}

void CScene::Update(float dt)
{
// 	for(vector<CRenderable*>::iterator i = RenderableObjects.begin(); i != RenderableObjects.end(); ++i)
// 	{
// 		(*i)->Color.a -= dt / 4.0f;
// 
// 	}
}

void CScene::AddRenderable(CRenderable *AObject)
{
	RenderableObjects.push_back(AObject);
	CRenderManager::Instance()->Add(AObject);
}

void CScene::AddUpdatable(CUpdatable *AObject)
{
	UpdatableObjects.push_back(AObject);
	CUpdateManager::Instance()->Add(AObject);
}

void CScene::RemoveUpdatable(CUpdatable *AObject)
{
	vector<CUpdatable *>::iterator it = find(UpdatableObjects.begin(), UpdatableObjects.end(), AObject);

	if (it == UpdatableObjects.end())
		return;
	
	UpdatableObjects.erase(it);

	//AObject->PutIntoScene()
}

void CScene::RemoveRenderable(CRenderable *AObject)
{
	vector<CRenderable *>::iterator it = find(RenderableObjects.begin(), RenderableObjects.end(), AObject);

	if (it == RenderableObjects.end())
		return;
	
	RenderableObjects.erase(it);
}

CScene::~CScene()
{
	for(vector<CUpdatable*>::iterator i = UpdatableObjects.begin(); i != UpdatableObjects.end(); ++i)
	{
		if ((*i)->Managed) 
			CFactory::Instance()->Destroy(*i);
	}
	for(vector<CRenderable*>::iterator i = RenderableObjects.begin(); i != RenderableObjects.end(); ++i)
	{
		if ((*i)->Managed) 
			CFactory::Instance()->Destroy(*i); 
	}
}

//////////////////////////////////////////////////////////////////////////
// CGlobalScene

void CGlobalScene::AddRenderable(CRenderable *AObject)
{
	CRenderManager::Instance()->Add(AObject);
}

void CGlobalScene::AddUpdatable(CUpdatable *AObject)
{
	CUpdateManager::Instance()->Add(AObject);
}

//////////////////////////////////////////////////////////////////////////
//CSceneManager

CSceneManager::CSceneManager() : CurrentScene(&GlobalScene)
{
	SetName("Scene manager");
}

void CSceneManager::Render()
{
	CurrentScene->Render();
}

void CSceneManager::Update(float dt)
{
	CurrentScene->Update(dt);
}

CAbstractScene* CSceneManager::GetCurrentScene()
{
	return CurrentScene;
}

bool CSceneManager::InScope(CAbstractScene *AScene) const
{
	// @todo: More complex check later.
	return CurrentScene == AScene || AScene == &GlobalScene;
}

CAbstractScene* CSceneManager::CreateScene()
{
	CAbstractScene* NewScene = new CScene();
	Scenes.push_back(NewScene);
	return NewScene;
}

CSceneManager::~CSceneManager()
{
	for(vector<CAbstractScene*>::iterator i = Scenes.begin(); i != Scenes.end(); ++i)
		delete (*i);
	Scenes.clear();
}

//////////////////////////////////////////////////////////////////////////
//CRenderProxy
CRenderProxy::CRenderProxy( CRenderable *ARenderSource ) : RenderSource(ARenderSource)
{
	SetName("CRenderProxy");
}

void CRenderProxy::Render()
{
	RenderSource->Render();
}