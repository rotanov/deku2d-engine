#include "2de_GraphicsLow.h"

#include "2de_Math.h"
#include "2de_Engine.h"

#if defined(USE_SDL_OPENGL)
	#include <SDL/SDL_opengl.h>
#elif defined(USE_GLEW)
	#define GLEW_STATIC //GLEW_BUILD
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif


//////////////////////////////////////////////////////////////////////////
// CRenderableUnitInfo

CRenderConfig::CRenderConfig() : Position(V2_ZERO), Color(COLOR_WHITE), doIgnoreCamera(false), Angle(0.0f), Depth(0.0f), Scaling(1.0f)
{
	
}

void CRenderConfig::SetAngle(float AAngle /*= 0.0f*/)
{
	if (AAngle > 360.0f)
		Angle = AAngle - (static_cast<int>(AAngle) / 360) * 360.0f; 
	else
		Angle = AAngle;
	//Angle = Clamp(AAngle, 0.0f, 360.0f);
}

float CRenderConfig::GetAngle() const
{
	return Angle;
}

void CRenderConfig::SetLayer(int Layer)
{
	Depth = Layer == 0 ? 0.0f : Layer / 100.0f;
}

float CRenderConfig::GetDepth() const
{
	return Depth;
}

float CRenderConfig::GetScaling() const
{
	return Scaling;
}

void CRenderConfig::SetScaling(float AScaling)
{
	Scaling = AScaling;
// 	Box.Min *= Scaling;
// 	Box.Max *= Scaling;
}

int CRenderConfig::GetLayer() const
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

// Danger: When we use it as arg to DrawSolidBox() then it apply scaling two times. @todo: fix this <--
const CBox CRenderable::GetBox() const
{
 	CBox TempBox = Box;
 	TempBox.Min *= GetScaling();
 	TempBox.Max *= GetScaling();
	TempBox.Offset(Position);
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

void CRenderable::SetVisibility(bool AVisible)
{
	Visible = AVisible;
}

float CRenderable::Width()
{
	return Box.Width();
}

float CRenderable::Height()
{
	return Box.Height();
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
	if (glIsTexture(TexID))
		glDeleteTextures(1, &TexID);
}

bool CGLImageData::MakeTexture()
{
	if (Data == NULL)
		return false;
	if ((Width & (Width - 1)) != 0)		//	Тут мы просто выходим, если ширина или высота  не является степенью двойки.
		return false;				//	Ultimate - это использовать NOT_POWER_OF_TWO екстеншон, если он доступен;
	if ((Height & (Height - 1)) != 0)	//	Иначе - дописывать в память кусок прозрачного цвета, по размеру такой, чтобы
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
		Log("ERROR", "Can't load texture in video memory");
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
		Log("ERROR", "Can't load texture in video memory");
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
	isFullscreen = false;
	BPP = 32;
	Caption = "Warning: CGLWindow class instance have not been initialized properly";
}

bool CGLWindow::gCreateWindow(bool AFullscreen, int AWidth, int AHeight, byte ABPP, const string &ACaption)
{
	isFullscreen = AFullscreen;
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

	int flags = SDL_OPENGL | SDL_RESIZABLE;// | SDL_NOFRAME;
	if (isFullscreen == true)
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

	GLenum glewError = glewInit();
	if (GLEW_OK != glewError)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		Log("ERROR", "%s", glewGetErrorString(glewError));
		return false;
	}

	SDL_Event resizeEvent;
	resizeEvent.type = SDL_VIDEORESIZE;
	resizeEvent.resize.w = Width;
	resizeEvent.resize.h = Height;
	SDL_PushEvent(&resizeEvent);

	//glCreateShader()

	/* Some code snippets for OpenGL-features aviability check:

		if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
		else {
		printf("Not totally ready :( \n");
		exit(1);

		if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
		else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
		}

		to use shaders we will go through the following scheme
		glCreateProgram();
			for each shader do ( glAttachShader(); )
		glLinkProgram();
		glUseProgram();

		and for each Shader do(
			glCreateShader();
			glShaderSource();
			glCompileShader();
		)	- now we can attach it.
	}
	*/

	glInit(Width, Height);

	CPrimitiveRender temp;
	temp.Init();
	isCreated = true;
	return true;
}

bool CGLWindow::gCreateWindow()
{
	return gCreateWindow(isFullscreen, Width, Height, BPP, Caption);
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
	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, Width, 0.0f, Height, -100.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void CGLWindow::glInit(GLsizei Width, GLsizei Height)
{
	glShadeModel(GL_SMOOTH);	//GL_SMOOTH GL_FLAT

	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClearColor(0.0, 0.0, 0.0, 0.5);
	SetBackgroundColor(RGBAf(0.0, 0.0, 0.0, 0.5));

	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, Width, 0.0f, Height, -100.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);

// 	glEnable(GL_POINT_SMOOTH);
// 	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
// 	glEnable(GL_LINE_SMOOTH);
// 	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
// 	glEnable(GL_POLYGON_SMOOTH);
// 	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	setVSync(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//(GL_SRC_ALPHA,GL_ONE)
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);
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

void CGLWindow::SetBPP(byte ABPP)
{
	BPP = ABPP;
}

void CGLWindow::SetFullscreen(bool AFullscreen)
{
	isFullscreen = AFullscreen;
}
//////////////////////////////////////////////////////////////////////////
// CFont

CFont::CFont() : Distance(1.0f)// Нет! Грузить её из файла!!! Ну, по крайне мере по умолчанию ставить из файла значение.// Пользователь потом сам попроавит, если надо.
{
	CFontManager::Instance()->Add(this);
}

CFont::~CFont()
{
	Unload();
	CFontManager::Instance()->Remove(this);
}

bool CFont::Load()
{
	if (Loaded)
		return true;

	if (Source == LOAD_SOURCE_FILE)
	{
		if (Filename == "")
			return false;

		CFile file;
		if (!file.Open(Filename, CFile::OPEN_MODE_READ))
		{
			Log("ERROR", "Can't Load Font '%s': file couldn't be opened", GetName().c_str());
			return false;
		}

		string FontImageName;
		file.ReadString(FontImageName);

		CTextureManager *TexMan = CTextureManager::Instance();
		Texture = TexMan->Get(FontImageName);

		if (Texture == NULL)
			return false;

		Texture->CheckLoad(); // я не помню, зачем я это сюда добавил, но у меня чёто падало без этого, хотя может и не из-за этого...

		// 	int Vertices[4*256];
		// 	file.Read(Vertices, sizeof(Vertices));
		// 	for(int i = 0; i < 256; i++)
		// 	{
		// 		bbox[i].Min.x = Vertices[i * 4 + 0];
		// 		bbox[i].Min.y = Vertices[i * 4 + 2];
		// 		bbox[i].Max.x = Vertices[i * 4 + 1];
		// 		bbox[i].Max.y = Vertices[i * 4 + 3];
		// 	}	// Для конвертирования из старого формата.
		file.Read(Boxes, sizeof(Boxes));

		file.Close();
		for (int i = 0; i < 256; i++)
		{
			if (Boxes[i].Min.x > Boxes[i].Max.x)
				swap(Boxes[i].Min.x, Boxes[i].Max.x);
			if (Boxes[i].Min.y > Boxes[i].Max.y)
				swap(Boxes[i].Min.y, Boxes[i].Max.y);
			Width[i] = (Boxes[i].Max.x - Boxes[i].Min.x);
			Height[i] = (Boxes[i].Max.y - Boxes[i].Min.y);
		}
	}
	else if (Source == LOAD_SOURCE_MEMORY)
	{
		// TODO: see issue 42, take away common code..
		if (MemoryLoadData == NULL || MemoryLoadLength == 0)
			return false;

		// assert(MemoryLoadData != NULL);

		while (*MemoryLoadData)
			MemoryLoadData++;

		MemoryLoadData++;

		Texture = CTextureManager::Instance()->Get("DefaultFontTexture");

		memcpy(Boxes, MemoryLoadData, sizeof(Boxes));
		for (int i = 0; i < 256; i++)
		{
			if (Boxes[i].Min.x > Boxes[i].Max.x)
				swap(Boxes[i].Min.x, Boxes[i].Max.x);
			if (Boxes[i].Min.y > Boxes[i].Max.y)
				swap(Boxes[i].Min.y, Boxes[i].Max.y);
			Width[i] = (Boxes[i].Max.x - Boxes[i].Min.x);
			Height[i] = (Boxes[i].Max.y - Boxes[i].Min.y);
		}
	}

	CResource::Load();

	return true;
}

void CFont::Unload()
{
	if (!Loaded)
		return;

	Texture = NULL;
	// well, nothing else in font are allocated dynamically, so nothing to clear..

	CResource::Unload();
}

bool CFont::SaveToFile(const string &AFilename)
{
	if (AFilename == "")
		return false;

	if (!Texture)
		return false;

	CFile file;
	file.Open(AFilename, CFile::OPEN_MODE_WRITE);
	file.Write(Texture->GetName().c_str(), Texture->GetName().length());
	file.WriteByte((byte)0x00);
	file.Write(Boxes, sizeof(Boxes));
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
	for (int i = 0; i < 256; i++)
	{
		Result.Add(Boxes[i].Min);
		Result.Add(Boxes[i].Max);
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

float CFont::SymbolWidth(unsigned int Index) const
{
	return Width[Index];
}

Vector2Array<4> CFont::GetTexCoords(unsigned int Charachter) /*const Vector2Array<4>& GetTexCoords(unsigned int Charachter) // <-- warning: reference to local variable result returned */
{
	Vector2Array<4> result;
	result[0] = Vector2(Boxes[Charachter - 32].Min.x / Texture->Width,
		Boxes[Charachter - 32].Min.y / Texture->Height);
	result[1] = Vector2(Boxes[Charachter - 32].Max.x / Texture->Width,
		Boxes[Charachter - 32].Min.y / Texture->Height);
	result[2] = Vector2(Boxes[Charachter - 32].Max.x / Texture->Width,
		Boxes[Charachter - 32].Max.y / Texture->Height);
	result[3] = Vector2(Boxes[Charachter - 32].Min.x / Texture->Width,
		Boxes[Charachter - 32].Max.y / Texture->Height);
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
	CRenderConfig temp;
	temp.doIgnoreCamera = true;
	CRenderManager::Instance()->DrawLinedBox(&temp, world);
	CRenderManager::Instance()->DrawLinedBox(&temp, view);
	CRenderManager::Instance()->DrawLinedBox(&temp, outer);
}

void CCamera::gTranslate()
{
	glTranslatef((int) (-Point.x + w/2.0f), (int) (-Point.y + h/2.0f), 0.0f);
}

CCamera::CCamera()
{
	SetName("CCamera");
	//view = world = CBox(100, 100, 540, 380);
	//outer = CBox(-1024, 0, 2048, 512);
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

//////////////////////////////////////////////////////////////////////////
// CRenderManager

CRenderManager::CRenderManager()
{
	SetName("Render Manager");
	Renderer = new CFFPRenderer();
}

CRenderManager::~CRenderManager()
{
	delete Renderer;
}

bool CRenderManager::DrawObjects()
{
	CRenderConfig TempRenderInfo;
	TempRenderInfo.SetLayer(512);
	TempRenderInfo.Color = COLOR_RED;
	TempRenderInfo.doIgnoreCamera = true;

	glLoadIdentity();	
	glTranslatef(0.0f, 0.0f, ROTATIONAL_AXIS_Z);

	Camera.Update(); // @todo: review camera

	CRenderable *data;
// 	CGameObject *GameObject = CUpdateManager::Instance()->RootGameObject;
// 	for(CGameObject::traverse_iterator_bfs i(*GameObject); i.Ok(); ++i)
// 	{
// 		CRenderableComponent *RenderComponent = dynamic_cast<CRenderableComponent *>(&(*i));
// 		if (RenderComponent != NULL)
// 		{
// 			Transformator.PushTransformation(&RenderComponent->Configuration);
// 			Renderer->PushModel(&RenderComponent->Configuration, RenderComponent->Model);
// 			Transformator.PopTransformation();
// 		}
// 	}

	TransfomationTraverse(CUpdateManager::Instance()->RootGameObject);

	for (ManagerConstIterator i = Objects.begin(); i != Objects.end(); ++i)
	{
		data = *i;
		if (data->isDestroyed() || !data->GetVisibility() ||
			!CSceneManager::Instance()->InScope(data->GetScene()))
			continue;
		data->Render();
#if defined(_2DE_DEBUG_DRAW_BOXES)
		DrawLinedBox(&TempRenderInfo, data->GetBox());
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, ROTATIONAL_AXIS_Z); //accuracy tip used
	glTranslatef(0.375, 0.375, 0);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	for (unsigned int i = 0; i < DEKU2D_MAX_TEXTURES; i++)
		if (TexturedQuadVertices[i].GetVertexCount() != 0)
		{
			glBindTexture(GL_TEXTURE_2D, i);
			TexturedQuadVertices[i].RenderPrimitive(GL_QUADS);
			TexturedQuadVertices[i].Clear();
		}
	//FontVertices.RenderPrimitive(GL_QUADS);
	Renderer->Render();
	Renderer->Clear();

	glDisable(GL_TEXTURE_2D);

	QuadVertices.RenderPrimitive(GL_QUADS);
	LineVertices.RenderPrimitive(GL_LINES);
	PointVertices.RenderPrimitive(GL_POINTS);
	PointVertices.Clear();
	LineVertices.Clear();
	QuadVertices.Clear();
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
		Camera.DrawDebug();
#endif 
	return true;
}

void CRenderManager::Print(const CText *Text, const string &Characters)
{
	CFont *Font = Text->GetFont();
	assert(Font != NULL);
	Font->CheckLoad();
	const CText &RText = *Text;
	RGBAf TColor = Text->Color;

	float dx = 0.0f;
	float dy = 0.0f;

	float liwidth;
	float liheight;
	float maxliheight = 0.0f;

	for (unsigned int i = 0; i < Text->Length(); i++)
	{
		if (RText[i] == '\n')
		{
			dx = 0.0f;
			dy -= maxliheight + 1; // FIXME: magic number (1) - vertical spacing
			maxliheight = 0.0f; // comment this if you want all lines to have the same (maximum) height..
			continue;
		}

		liwidth = Font->Boxes[RText[i] - 32].Width();	// 32!!!
		liheight = Font->Boxes[RText[i] - 32].Height();
		maxliheight = max(maxliheight, liheight);

		Vector2Array<4> TempTexCoords = Font->GetTexCoords(RText[i]);

		TexturedQuadVertices[Font->GetTexture()->GetTexID()].PushVertex(Text, Vector2(dx, dy), TColor, TempTexCoords[0]);
		TexturedQuadVertices[Font->GetTexture()->GetTexID()].PushVertex(Text, Vector2(liwidth + dx, dy), TColor, TempTexCoords[1]);
		TexturedQuadVertices[Font->GetTexture()->GetTexID()].PushVertex(Text, Vector2(liwidth + dx, liheight + dy), TColor, TempTexCoords[2]);
		TexturedQuadVertices[Font->GetTexture()->GetTexID()].PushVertex(Text, Vector2(dx, liheight + dy), TColor, TempTexCoords[3]);

		dx += Font->Boxes[RText[i] - 32].Width() + 1; // FIXME: magic number (1) - horizontal spacing
	}
}

void CRenderManager::DrawLinedBox(const CRenderConfig* RenderInfo, const CBox &Box)
{
	Vector2 v0 = Box.Min;
	Vector2 v1 = Vector2(Box.Max.x, Box.Min.y);
	Vector2 v2 = Box.Max;
	Vector2 v3 = Vector2(Box.Min.x, Box.Max.y);
	LineVertices.PushVertex(RenderInfo, v0);
	LineVertices.PushVertex(RenderInfo, v1);
	LineVertices.PushVertex(RenderInfo, v1);
	LineVertices.PushVertex(RenderInfo, v2);
	LineVertices.PushVertex(RenderInfo, v2);
	LineVertices.PushVertex(RenderInfo, v3);
	LineVertices.PushVertex(RenderInfo, v3);
	LineVertices.PushVertex(RenderInfo, v0);
}

void CRenderManager::DrawSolidBox(const CRenderConfig* RenderInfo, const CBox &Box)
{
	Vector2 v0 = Box.Min;
	Vector2 v1 = Vector2(Box.Max.x, Box.Min.y);
	Vector2 v2 = Box.Max;
	Vector2 v3 = Vector2(Box.Min.x, Box.Max.y);
	QuadVertices.PushVertex(RenderInfo, v0);
	QuadVertices.PushVertex(RenderInfo, v1);
	QuadVertices.PushVertex(RenderInfo, v2);
	QuadVertices.PushVertex(RenderInfo, v3);
}

void CRenderManager::DrawTexturedBox(const CRenderConfig *RenderInfo, const CBox &Box, CTexture *Texture, const Vector2Array<4> &TexCoords)
{
	Vector2 v0 = Box.Min;	
	Vector2 v1 = Vector2(Box.Max.x, Box.Min.y);
	Vector2 v2 = Box.Max;
	Vector2 v3 = Vector2(Box.Min.x, Box.Max.y);

	unsigned int i = Texture->GetTexID();
	TexturedQuadVertices[i].PushVertex(RenderInfo, v0, TexCoords[0]);
	TexturedQuadVertices[i].PushVertex(RenderInfo, v1, TexCoords[1]);
	TexturedQuadVertices[i].PushVertex(RenderInfo, v2, TexCoords[2]);
	TexturedQuadVertices[i].PushVertex(RenderInfo, v3, TexCoords[3]);
}

void CRenderManager::DrawPoint(const CRenderConfig *RenderInfo, const Vector2 &Point)
{
	PointVertices.PushVertex(RenderInfo, Point);
}

void CRenderManager::DrawLine(const CRenderConfig *RenderInfo, const Vector2 &v0, const Vector2 &v1)
{
	LineVertices.PushVertex(RenderInfo, v0);
	LineVertices.PushVertex(RenderInfo, v1);
}

void CRenderManager::DrawTriangles(const CRenderConfig *RenderInfo, const Vector2 *Vertices, unsigned int Count)
{
	// ... 
}

void CRenderManager::BeginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CRenderManager::EndFrame()
{
	glFinish();
	SDL_GL_SwapBuffers();
}

void CRenderManager::TransfomationTraverse(CGameObject *Next)
{
	CRenderableComponent *RenderComponent = dynamic_cast<CRenderableComponent *>(Next);
	if (RenderComponent != NULL)
		Transformator.PushTransformation(&RenderComponent->Configuration);
	for(unsigned int i = 0; i < Next->Children.size(); i++)
	{
		CRenderableComponent *RenderComponent = dynamic_cast<CRenderableComponent *>(Next->Children[i]);
		if (RenderComponent != NULL)
		{
			Transformator.PushTransformation(&RenderComponent->Configuration);
			Renderer->PushModel(&RenderComponent->Configuration, RenderComponent->Model);
			Transformator.PopTransformation();
		}
	}
	for(unsigned int i = 0; i < Next->Children.size(); i++)
		TransfomationTraverse(Next->Children[i]);
	if (RenderComponent != NULL)
		Transformator.PopTransformation();
}

//////////////////////////////////////////////////////////////////////////
// CFontManager

CFontManager::CFontManager() : DefaultFont(NULL)
{
	SetName("Font manager");
}


void CFontManager::Init()
{
	CTexture* DefaultFontTexture = CFactory::Instance()->New<CTexture>("DefaultFontTexture");
	DefaultFontTexture->LoadTexture(IMAGE_DEFAULT_FONT_WIDTH, IMAGE_DEFAULT_FONT_HEIGHT, reinterpret_cast<byte *>(IMAGE_DEFAULT_FONT_DATA));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// wtf is it? incapsulate it please or whatever...
	
	DefaultFont = CFactory::Instance()->New<CFont>("DefaultFont");
	DefaultFont->SetLoadSource(reinterpret_cast<byte *>(BINARY_DATA_DEFAULT_FONT), BINARY_DATA_DEFAULT_FONT_SIZE);
	DefaultFont->SetPersistent(true);
	DefaultFont->Load(); // maybe no need to explicitly call Load here..

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

CFontManager::~CFontManager()
{
}

CFont* CFontManager::GetDefaultFont()
{
	assert(DefaultFont != NULL);
	return DefaultFont;
}
//////////////////////////////////////////////////////////////////////////
// CTexture Manager
CTextureManager::CTextureManager()
{
	SetName("Texture manager");
}

//////////////////////////////////////////////////////////////////////////
// CTexture
GLuint CTexture::GetTexID()
{
	assert(TexID != 0);
	if (TexID == 0)
	{
		Log("ERROR", "CTexture named %s. Trying to access TexID but it is 0", GetName().c_str());
		Load();
	}
	return TexID;
}

CTexture::CTexture()
{
	SetName("CTexture");
	CTextureManager::Instance()->Add(this);
}

CTexture::~CTexture()
{
	Unload();
	CTextureManager::Instance()->Remove(this);
}


bool CTexture::Load()
{
	if (Loaded)
		return true;

	if (Source == LOAD_SOURCE_FILE)
	{
		if (Filename == "")
		{
			// TODO: fix and simplify error message..
			Log("ERROR", "Trying to load texture with name %s; But it has not been found in ResourceList(s)\n\t or Resource List Has not been loaded", GetName().c_str());
			return false;
		}

		if (!LoadTexture(Filename))
			return false;
	}
	else if (Source = LOAD_SOURCE_MEMORY)
	{
		// TODO: implement loading from memory..
		throw std::logic_error("Unimplemented!!");
	}

	CResource::Load();

	return true;
}

void CTexture::Unload()
{
	if (!Loaded)
		return;

	glDeleteTextures(1, &TexID);

	CResource::Unload();
}

bool CTexture::SaveToFile(const string &AFilename)
{
	// TODO: implement this..
	throw std::logic_error("Unimplemented!!");
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
		Log("ERROR", SWAP_INTERVAL_EXTENSION_NAME " is not supported on your computer");
	}
#endif                        // </platforms, that support swap interval control>
}

//////////////////////////////////////////////////////////////////////////
// CText

CText::CText() : Font(CFontManager::Instance()->GetDefaultFont())
{	
	assert(Font != NULL);
	SetText("");
	doIgnoreCamera = true;
}

CText::CText(const string &AText) : Font(CFontManager::Instance()->GetDefaultFont())
{
	assert(Font != NULL);
	SetText(Characters);
	doIgnoreCamera = true;
}
void CText::Render()
{
	CRenderManager::Instance()->Print(this, Characters);
}

CFont* CText::GetFont() const
{
	assert(Font != NULL);
	return Font;
}

string& CText::GetText()
{
	return Characters;
}

const string& CText::GetText() const
{
	return Characters;
}

void CText::SetFont(CFont *AFont)
{
	assert(AFont != NULL);
	Font = AFont;
	SetText(Characters);	// Пересчитать BOX, шрифт же меняется.
}

void CText::SetText(const string &AText)
{
	Characters = AText;
	SetBox(CBox(Position, Position + Vector2(Font->GetStringWidth(Characters), Font->GetStringHeight(Characters))));
}

CText& CText::operator =(const string &AText)
{
	SetText(AText);
	return *this;
}

float CText::StringCoordToCursorPos(int x, int y) const
{
	if (Characters.length() == 0)
		return -1;
	Vector2 Local = Vector2(x, y) - Position;
	if (Local.x < 0)
		return -1;

	for (int i = 0; i < Characters.length(); i++)
	{
		float SubstrWidth = Font->GetStringWidthEx(0, i, Characters);
		float SymbolCenterCoord = SubstrWidth - Font->GetDistance() - (Font->SymbolWidth((byte)Characters[i] - 32) / 2.0f);
		if (Local.x < SymbolCenterCoord)
			return (i - 1);
	}
	return (Characters.length() - 1.0f);
}

CText::~CText()
{
	
}

unsigned char CText::operator[](unsigned int index) const
{
	return Characters[index];
}

unsigned int CText::Length() const
{
	return Characters.length();
}


//////////////////////////////////////////////////////////////////////////
// CAbstractScene

CAbstractScene::CAbstractScene()
{
	SetName("Some abstract scene");
}

//////////////////////////////////////////////////////////////////////////
// CScene

void CScene::Render()
{
	// блеадь, дублирование кода из CRenderManager::draw
	// АХАХА, его нет.
}

void CScene::Update(float dt)
{
// 	for (vector<CRenderable*>::iterator i = RenderableObjects.begin(); i != RenderableObjects.end(); ++i)
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
	for (vector<CUpdatable*>::iterator i = UpdatableObjects.begin(); i != UpdatableObjects.end(); ++i)
	{
		(*i)->SetDestroyed();
	}
	for (vector<CRenderable*>::iterator i = RenderableObjects.begin(); i != RenderableObjects.end(); ++i)
	{
		(*i)->SetDestroyed();
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
// CSceneManager

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
	for (vector<CAbstractScene*>::iterator i = Scenes.begin(); i != Scenes.end(); ++i)
		delete (*i);
	Scenes.clear();
}

void CSceneManager::SetCurrentScene(CAbstractScene *AScene)
{
	assert(AScene != NULL);
	CurrentScene = AScene;
}
//////////////////////////////////////////////////////////////////////////
// CRenderProxy

CRenderProxy::CRenderProxy(CRenderable *ARenderSource) : RenderSource(ARenderSource)
{
	SetName("CRenderProxy");
}

void CRenderProxy::Render()
{
	RenderSource->Render();
}

//////////////////////////////////////////////////////////////////////////
// CPrimitiveVertexDataHolder

CPrmitiveVertexDataHolder::CPrmitiveVertexDataHolder() : VertexCount(0), ReservedCount(StartSize), Colors(NULL), Vertices(NULL)
{
	Colors = new RGBAf [StartSize];
	Vertices = new Vector3 [StartSize];
}

CPrmitiveVertexDataHolder::~CPrmitiveVertexDataHolder()
{
	delete [] Colors;
	delete [] Vertices;
}

void CPrmitiveVertexDataHolder::PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex, const RGBAf &Color)
{
	assert(Sender != NULL);
	if (VertexCount == ReservedCount)
		Grow();
	Vector2 TempVector = (Vertex * Sender->GetScaling());
	if (!Equal(Sender->GetAngle(), 0.0f))
		TempVector *= Matrix2(DegToRad(-Sender->GetAngle()));
	TempVector += Sender->Position;
	if (!Sender->doIgnoreCamera)
		TempVector += CRenderManager::Instance()->Camera.GetTranslation();
	Vertices[VertexCount] = Vector3(static_cast<int>(TempVector.x), static_cast<int>(TempVector.y), Sender->GetDepth());
	Colors[VertexCount] = Color;
	VertexCount++;
}

void CPrmitiveVertexDataHolder::PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex)
{
	PushVertex(Sender, Vertex, Sender->Color);
}

void CPrmitiveVertexDataHolder::RenderPrimitive(GLenum Type)
{
	if (VertexCount == 0)
		return;
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, Vertices);
	glColorPointer(4, GL_FLOAT, 0, Colors);
	glDrawArrays(Type, 0, VertexCount);
}

void CPrmitiveVertexDataHolder::Grow()
{
	ReservedCount = VertexCount * 2;
	RGBAf *NewColors = new RGBAf[ReservedCount];
	for (unsigned int i = 0; i < VertexCount; i++)
		NewColors[i] = Colors[i];

	delete[] Colors;

	Colors = NewColors;

	Vector3 *NewVertices = new Vector3[ReservedCount];
	for (unsigned int i = 0; i < VertexCount; i++)
		NewVertices[i] = Vertices[i];

	delete[] Vertices;

	Vertices = NewVertices;
}

void CPrmitiveVertexDataHolder::Clear()
{
	VertexCount = 0;
}

unsigned int CPrmitiveVertexDataHolder::GetVertexCount()
{
	return VertexCount;
}
//////////////////////////////////////////////////////////////////////////
// CVertexDataHolder

CVertexDataHolder::CVertexDataHolder()
{
	TexCoords = new Vector2 [StartSize];
}

CVertexDataHolder::~CVertexDataHolder()
{
	delete [] TexCoords;
}

void CVertexDataHolder::PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex, const RGBAf &Color)
{
	assert(false);
}

void CVertexDataHolder::PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex, const RGBAf &Color, const Vector2 &TexCoord)
{
	CPrmitiveVertexDataHolder::PushVertex(Sender, Vertex, Color);
	if (VertexCount == ReservedCount)
		Grow();
	TexCoords[CPrmitiveVertexDataHolder::VertexCount - 1] = TexCoord;
}

void CVertexDataHolder::PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex)
{
	assert(false);
}

void CVertexDataHolder::PushVertex(const CRenderConfig *Sender, const Vector2 &Vertex, const Vector2 &TexCoord)
{
	PushVertex(Sender, Vertex, Sender->Color, TexCoord);
}

void CVertexDataHolder::RenderPrimitive( GLenum Type )
{
	if (VertexCount == 0)
		return;
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, CPrmitiveVertexDataHolder::Vertices);
	glColorPointer(4, GL_FLOAT, 0, CPrmitiveVertexDataHolder::Colors);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoords);
	glDrawArrays(Type, 0, CPrmitiveVertexDataHolder::VertexCount);
}

void CVertexDataHolder::Grow()
{
    CPrmitiveVertexDataHolder::Grow();
	Vector2 *NewTexCoords = new Vector2[ReservedCount];
	for (unsigned int i = 0; i < VertexCount; i++)
		NewTexCoords[i] = TexCoords[i];

	delete[] TexCoords;

	TexCoords = NewTexCoords;
}

//////////////////////////////////////////////////////////////////////////
// CRenderer



bool CFFPRenderer::Initilize()
{
	// do something
	return true;
}

bool CFFPRenderer::Finalize()
{
	// do something either
	return false;
}

void CFFPRenderer::PushModel(CRenderConfig *Sender, CModel * AModel)
{
	if (AModel == NULL)
		return;
	assert(Sender != NULL && AModel != NULL);
	CBetterVertexHolder *VertexHolder = &PrimitiveHolders[AModel->GetModelType()];

	if (AModel->GetTexture() != NULL)
	{
		bool flag = false;
		int index = -1;
		for(unsigned int i = 0; i < TexIDs.size(); i++)
			if (TexIDs[i] == AModel->GetTexture()->GetTexID())
			{
				flag = true;
				index = i;
				break;
			}

		if (!flag)
		{
			TexturedGeometry.push_back(new CBetterTextureVertexHolder());
			TexIDs.push_back(AModel->GetTexture()->GetTexID());
			index = TexIDs.size() - 1;
		}

		CBetterTextureVertexHolder * VertexHolder = TexturedGeometry[index];

		Vector2 TempVector = V2_ZERO, Vertex = V2_ZERO;
		CTransformation Transformation = CRenderManager::Instance()->Transformator.GetCurrentTransfomation();
		for(unsigned int i  = 0; i < AModel->GetVertexNumber(); i++)
		{
			Vertex = AModel->GetVertices()[i];
			TempVector = (Vertex * Transformation.GetScaling());
			if (!Equal(Transformation.GetAngle(), 0.0f))
				TempVector *= Matrix2(DegToRad(-Transformation.GetAngle()));
			TempVector += Transformation.GetPosition();//Sender->Position;
			if (!Sender->doIgnoreCamera)
				TempVector += CRenderManager::Instance()->Camera.GetTranslation();
			VertexHolder->PushVertex(Vector3
										(
											static_cast<int>(TempVector.x),
											static_cast<int>(TempVector.y),
											Transformation.GetDepth()
										),
									Sender->Color, AModel->GetTexCoords()[i]);
		}
		return;
	}

	switch (AModel->GetModelType())
	{
	case MODEL_TYPE_NOT_A_MODEL:
		assert(false);
		break;
	case MODEL_TYPE_POINTS:

		break;
	case MODEL_TYPE_LINES:

		break;
	case MODEL_TYPE_TRIANGLES:
		break;
	default:
		assert(false);
		break;
	}

	Vector2 TempVector = V2_ZERO, Vertex = V2_ZERO;
	CTransformation Transformation = CRenderManager::Instance()->Transformator.GetCurrentTransfomation();
	for(unsigned int i  = 0; i < AModel->GetVertexNumber(); i++)
	{
		Vertex = AModel->GetVertices()[i];
		TempVector = (Vertex * Transformation.GetScaling());
		if (!Equal(Transformation.GetAngle(), 0.0f))
			TempVector *= Matrix2(DegToRad(-Transformation.GetAngle()));
		TempVector += Transformation.GetPosition();//Sender->Position;
		if (!Sender->doIgnoreCamera)
			TempVector += CRenderManager::Instance()->Camera.GetTranslation();
		VertexHolder->PushVertex(Vector3(static_cast<int>(TempVector.x), static_cast<int>(TempVector.y), Transformation.GetDepth()), Sender->Color);
	}

}

void CFFPRenderer::Render()
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_POINTS);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	PrimitiveHolders[MODEL_TYPE_POINTS].RenderPrimitive(GL_POINTS);
	glEnable(GL_BLEND);
	glEnable(GL_LINES);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	PrimitiveHolders[MODEL_TYPE_LINES].RenderPrimitive(GL_LINES);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);	
	PrimitiveHolders[MODEL_TYPE_TRIANGLES].RenderPrimitive(GL_TRIANGLES);

	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);

	glEnable(GL_TEXTURE_2D);
	for(unsigned i = 0; i < TexturedGeometry.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, TexIDs[i]);
		TexturedGeometry[i]->RenderPrimitive(GL_TRIANGLES);
		TexturedGeometry[i]->Clear();
	}

	PrimitiveHolders[0].Clear();
	PrimitiveHolders[1].Clear();
	PrimitiveHolders[2].Clear();
}

CFFPRenderer::~CFFPRenderer()
{

}

void CFFPRenderer::Clear()
{
	for(unsigned int i = 0; i < MODEL_TYPE_TRIANGLES + 1; i++)
		PrimitiveHolders[i].Clear();
}

CFFPRenderer::CFFPRenderer()
{

}
//////////////////////////////////////////////////////////////////////////
// Better vertex holder


CFFPRenderer::CBetterVertexHolder::CBetterVertexHolder() : VertexCount(0), ReservedCount(StartSize)
{
	Colors = new RGBAf [StartSize];
	Vertices = new Vector3 [StartSize];	
}

CFFPRenderer::CBetterVertexHolder::~CBetterVertexHolder()
{
	delete [] Colors;
	delete [] Vertices;
}

void CFFPRenderer::CBetterVertexHolder::PushVertex(const Vector3 &AVertex, const RGBAf &AColor)
{
	if (VertexCount == ReservedCount)
		_Grow();
	Vertices[VertexCount] = AVertex;
	Colors[VertexCount] = AColor;
	VertexCount++;
}

void CFFPRenderer::CBetterVertexHolder::RenderPrimitive(GLuint Type)
{
	if (VertexCount == 0)
		return;
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, Vertices);
	glColorPointer(4, GL_FLOAT, 0, Colors);
	glDrawArrays(Type, 0, VertexCount);
}

void CFFPRenderer::CBetterVertexHolder::Clear()
{
	VertexCount = 0;
}

unsigned int CFFPRenderer::CBetterVertexHolder::GetVertexCount()
{
	return VertexCount;
}

void CFFPRenderer::CBetterVertexHolder::_Grow()
{
	ReservedCount = VertexCount * 2;
	RGBAf *NewColors = new RGBAf[ReservedCount];
	for (unsigned int i = 0; i < VertexCount; i++)
		NewColors[i] = Colors[i];

	delete[] Colors;

	Colors = NewColors;

	Vector3 *NewVertices = new Vector3[ReservedCount];
	for (unsigned int i = 0; i < VertexCount; i++)
		NewVertices[i] = Vertices[i];

	delete[] Vertices;

	Vertices = NewVertices;
}

//////////////////////////////////////////////////////////////////////////
// Transformation

CTransformation::CTransformation(float ADepthOffset, const Vector2 &ATranslation,
	float ARotation, float AScaling) : DepthOffset(ADepthOffset),
	Translation(ATranslation), Rotation(ARotation), Scaling(AScaling)
{

}

CTransformation& CTransformation::operator+=(const CTransformation &rhs)
{
	DepthOffset += rhs.DepthOffset;
	Translation += rhs.Translation;
	Rotation += rhs.Rotation;
	Scaling *= rhs.Scaling;
	return *this;
}

CTransformation& CTransformation::operator-=(const CTransformation &rhs)
{
	DepthOffset -= rhs.DepthOffset;
	Translation -= rhs.Translation;
	Rotation -= rhs.Rotation;
	Scaling /= rhs.Scaling;
	return *this;
}

float CTransformation::GetDepth() const
{
	return DepthOffset;
}

const Vector2& CTransformation::GetPosition() const
{
	return Translation;
}

float CTransformation::GetAngle() const
{
	return Rotation;
}

float CTransformation::GetScaling() const
{
	return Scaling;
}

void CTransformation::Clear()
{
	DepthOffset = 0.0f;
	Translation = V2_ZERO;
	Scaling = 1.0f;
	Rotation = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
// Transformator

void CTransformator::PushTransformation(const CRenderConfig * ATransformation)
{
	CTransformation TempTransformation(ATransformation->GetDepth(), ATransformation->Position,
		ATransformation->GetAngle(), ATransformation->GetScaling());
	CurrentTransformation += TempTransformation;
	TransformationStack.push_back(TempTransformation);
}

void CTransformator::PopTransformation()
{
	CurrentTransformation -= TransformationStack.back();
	TransformationStack.pop_back();
}

void CTransformator::ClearTransformation()
{
	TransformationStack.clear();
	CurrentTransformation.Clear();
}

CTransformator::CTransformator() : CurrentTransformation()
{

}

const CTransformation& CTransformator::GetCurrentTransfomation() const
{
	return CurrentTransformation;
}
//////////////////////////////////////////////////////////////////////////
// CModel



//////////////////////////////////////////////////////////////////////////
// чотатам
CFFPRenderer::CBetterTextureVertexHolder::CBetterTextureVertexHolder() : VertexCount(0), ReservedCount(StartSize) 
{
	Colors = new RGBAf [StartSize];
	Vertices = new Vector3 [StartSize];
	TexCoords = new Vector2[StartSize];
}

CFFPRenderer::CBetterTextureVertexHolder::~CBetterTextureVertexHolder()
{
	delete [] Colors;
	delete [] Vertices;
	delete [] TexCoords;
}

void CFFPRenderer::CBetterTextureVertexHolder::PushVertex(const Vector3 &AVertex, const RGBAf &AColor, const Vector2 &ATexCoord)
{
	if (VertexCount == ReservedCount)
		_Grow();
	Vertices[VertexCount] = AVertex;
	Colors[VertexCount] = AColor;
	TexCoords[VertexCount] = ATexCoord;
	VertexCount++;
}

void CFFPRenderer::CBetterTextureVertexHolder::RenderPrimitive(GLuint Type)
{
	if (VertexCount == 0)
		return;
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer(3, GL_FLOAT, 0, Vertices);
	glColorPointer(4, GL_FLOAT, 0, Colors);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoords);
	glDrawArrays(Type, 0, VertexCount);

}

void CFFPRenderer::CBetterTextureVertexHolder::Clear()
{
	VertexCount = 0;
}

unsigned int CFFPRenderer::CBetterTextureVertexHolder::GetVertexCount()
{
	return VertexCount;
}

void CFFPRenderer::CBetterTextureVertexHolder::_Grow()
{
	ReservedCount = VertexCount * 2;
	RGBAf *NewColors = new RGBAf[ReservedCount];
	for (unsigned int i = 0; i < VertexCount; i++)
		NewColors[i] = Colors[i];

	delete[] Colors;

	Colors = NewColors;

	Vector3 *NewVertices = new Vector3[ReservedCount];
	for (unsigned int i = 0; i < VertexCount; i++)
		NewVertices[i] = Vertices[i];

	delete[] Vertices;

	Vertices = NewVertices;

	Vector2 *NewTexCoords = new Vector2[ReservedCount];
	for (unsigned int i = 0; i < VertexCount; i++)
		NewTexCoords[i] = TexCoords[i];

	delete [] TexCoords;

	TexCoords = NewTexCoords;
}
