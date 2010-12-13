#include "2de_GraphicsLow.h"

#include "2de_Math.h"
#include "2de_Engine.h"

#if defined(USE_SDL_OPENGL)
	#include <SDL/SDL_opengl.h>
#elif defined(USE_GLEW)
	#define GLEW_STATIC //GLEW_BUILD
	#include <GL/glew.h>

	#if defined(_WIN32)
		#include <GL/wglew.h>
	#elif defined(__linux) // not only linux, i think.. any platform, that uses GLX..
		#include <GL/glxew.h>
	#endif
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

//////////////////////////////////////////////////////////////////////////
// CRenderableUnitInfo

CRenderConfig::CRenderConfig() : BlendingMode(BLEND_MODE_OPAQUE), Color(COLOR_WHITE)
{
}


void CRenderConfig::SetBlendingMode(EBlendingMode ABlendingMode)
{
	BlendingMode = ABlendingMode;
}

EBlendingMode CRenderConfig::GetBlendingMode() const
{
	return BlendingMode;
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
	return isCreated = true;
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

	CRenderManager::Instance()->SetSwapInterval(0);

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
	// ah, i got it.. see issue 44 then..

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
	glClearColor(AColor.r, AColor.g, AColor.b, AColor.a);
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

		while ((*static_cast<byte*>(MemoryLoadData)))
			MemoryLoadData = static_cast<byte*>(MemoryLoadData) + 1;
		MemoryLoadData = static_cast<byte*>(MemoryLoadData) + 1;

		Texture = CTextureManager::Instance()->Get("DefaultFontTexture");

		memcpy(Boxes, static_cast<byte*>(MemoryLoadData), sizeof(Boxes));
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
	else
	{
		Log("ERROR", "Can't load font: no load source specified");
		return false;
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
// 	CRenderConfig TempRenderInfo;
// 	TempRenderInfo.SetLayer(512);
// 	TempRenderInfo.Color = COLOR_RED;
// 	TempRenderInfo.doIgnoreCamera = true;
	glLoadIdentity();	
	glTranslatef(0.0f, 0.0f, ROTATIONAL_AXIS_Z);
	Camera.Update(); // @todo: review camera

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
/*
	for (ManagerConstIterator i = Objects.begin(); i != Objects.end(); ++i)
	{
		data = *i;
#if defined(_2DE_DEBUG_DRAW_BOXES)
		DrawLinedBox(&TempRenderInfo, data->GetBox());
#endif
	}
*/	// stands here to hold some logic

	//////////////////////////////////////////////////////////////////////////
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, ROTATIONAL_AXIS_Z); //accuracy tip used
	glTranslatef(0.375, 0.375, 0);
	Renderer->Render();
	Renderer->Clear();
	return true;
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
	CPlaceableComponent *PlaceableComponent = NULL;
	CRenderableComponent *RenderableComponent = NULL;
	for(unsigned int i = 0; i < Next->Children.size(); i++)
	{
		RenderableComponent = dynamic_cast<CRenderableComponent *>(Next->Children[i]);
		if (RenderableComponent != NULL && RenderableComponent->GetVisibility() && 
			!RenderableComponent->isDestroyed() && CSceneManager::Instance()->InScope(RenderableComponent->GetScene()))
		{
			Renderer->PushModel(&RenderableComponent->GetConfiguration(), RenderableComponent->GetModel());
			TransfomationTraverse(Next->Children[i]);
			continue;
		}

		PlaceableComponent = dynamic_cast<CPlaceableComponent *>(Next->Children[i]);
		if (PlaceableComponent != NULL && !PlaceableComponent->isDestroyed() &&
			CSceneManager::Instance()->InScope(PlaceableComponent->GetScene()))
		{
			Transformator.PushTransformation(PlaceableComponent->GetTransformation());

			TransfomationTraverse(Next->Children[i]);

			Transformator.PopTransformation();

			continue;
		}

		if (!PlaceableComponent && !RenderableComponent)
			TransfomationTraverse(Next->Children[i]);
	}
}

CModel* CRenderManager::CreateModelText(const CText *AText)
{
	CFont *Font = AText->GetFont();
	assert(Font != NULL);
	Font->CheckLoad();
	const CText &RText = *AText;
	Vector2 *Vertices = new Vector2 [6 * AText->Length()];
	Vector2 *TexCoords = new Vector2 [6 * AText->Length()];

	float dx = 0.0f;
	float dy = 0.0f;

	float liwidth;
	float liheight;
	float maxliheight = 0.0f;

	for (unsigned int i = 0; i < AText->Length(); i++)
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

		Vertices[i * 6 + 0] = Vector2(dx, dy);
		Vertices[i * 6 + 1] = Vector2(liwidth + dx, dy);
		Vertices[i * 6 + 2] = Vector2(liwidth + dx, liheight + dy);

		Vertices[i * 6 + 3] = Vector2(dx, dy);
		Vertices[i * 6 + 4] = Vector2(liwidth + dx, liheight + dy);
		Vertices[i * 6 + 5] = Vector2(dx, liheight + dy);

		TexCoords[i * 6 + 0] = TempTexCoords[0];
		TexCoords[i * 6 + 1] = TempTexCoords[1];
		TexCoords[i * 6 + 2] = TempTexCoords[2];

		TexCoords[i * 6 + 3] = TempTexCoords[0];
		TexCoords[i * 6 + 4] = TempTexCoords[2];
		TexCoords[i * 6 + 5] = TempTexCoords[3];

		dx += Font->Boxes[RText[i] - 32].Width() + 1; // FIXME: magic number (1) - horizontal spacing
	}
	CModel *Model = new CModel(MODEL_TYPE_TRIANGLES, AText->GetFont()->GetTexture(), 6 * AText->Length(), Vertices, TexCoords);
	CFactory::Instance()->Add(Model);
	delete [] Vertices;
	delete [] TexCoords;
	return Model;
}

void CRenderManager::SetSwapInterval(int interval /*= 1*/)
{
#if defined(_WIN32)
	if (WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(interval);
		return;
	}
#elif defined(__linux)
	if (GLXEW_SGI_swap_control)
	{
		glXSwapIntervalSGI(interval);
		return;
	}
#endif
	Log("ERROR", "swap_control is not supported on your computer");
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
	DefaultFontTexture->SetLoadSource(BINARY_DATA_FONT_FONT, BINARY_DATA_FONT_FONT_SIZE);
	//DefaultFontTexture->LoadTexture(IMAGE_DEFAULT_FONT_WIDTH, IMAGE_DEFAULT_FONT_HEIGHT, reinterpret_cast<byte *>(IMAGE_DEFAULT_FONT_DATA));
	DefaultFontTexture->Load();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// wtf is it? incapsulate it please or whatever...
	
	DefaultFont = CFactory::Instance()->New<CFont>("DefaultFont");
	DefaultFont->SetLoadSource(reinterpret_cast<byte *>(BINARY_DATA_DEFAULT_FONT), BINARY_DATA_FONT_FONT_SIZE);
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
// CTextureManager

CTextureManager::CTextureManager()
{
	SetName("Texture manager");
}

//////////////////////////////////////////////////////////////////////////
// CTexture

bool CTexture::MakeTexture()
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

GLuint CTexture::GetTexID()
{
	if (TexID == 0)
		Log("ERROR", "GLImageData. Trying to access TexID but it is 0");
	return TexID;
}

GLuint CTexture::GetTexID() const
{
	assert(TexID != 0);
	if (TexID == 0)
	{
		Log("ERROR", "CTexture named %s. Trying to access TexID but it is 0", GetName().c_str());
		const_cast<CTexture*>(this)->Load();
	}
	return TexID;
}

CTexture::CTexture()
{
	TexID = Width = Height = BPP = 0;
	doCleanData = true;
	Data = NULL;
	SetName("CTexture");
	CTextureManager::Instance()->Add(this);
}

CTexture::~CTexture()
{
	if (glIsTexture(TexID))
		glDeleteTextures(1, &TexID);
	Unload();
	CTextureManager::Instance()->Remove(this);
}


bool CTexture::Load()
{
	if (Loaded)
		return true;

	if (Source == LOAD_SOURCE_FILE)
	{
		if (!LoadFromFile(Filename))
		{
			Log("ERROR", "Can't load texture from a file");
			return false;
		}
	}
	else if (Source = LOAD_SOURCE_MEMORY)
	{
		if (!LoadFromMemory(MemoryLoadData, MemoryLoadLength))
		{
			Log("ERROR", "Can't load image from memory");
			return false;
		}
	}
	else
	{
		Log("ERROR", "Can't load texture: no load source specified");
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


//////////////////////////////////////////////////////////////////////////
// CAbstractScene

CAbstractScene::CAbstractScene()
{
	SetName("Some abstract scene");
}

//////////////////////////////////////////////////////////////////////////
// CScene

void CScene::Add(CGameObject *AObject)
{
	GameObjects.push_back(AObject);
	//CRenderManager::Instance()->Add(AObject);
}


void CScene::Remove(CGameObject *AObject)
{
	vector<CGameObject *>::iterator it = find(GameObjects.begin(), GameObjects.end(), AObject);

	if (it == GameObjects.end())
		return;
	
	GameObjects.erase(it);
}

CScene::~CScene()
{
	for (vector<CGameObject*>::iterator i = GameObjects.begin(); i != GameObjects.end(); ++i)
	{
		(*i)->SetDestroyed();
	}
}

//////////////////////////////////////////////////////////////////////////
// CGlobalScene

void CGlobalScene::Add(CGameObject *AObject)
{
	//CRenderManager::Instance()->Add(AObject);
}

//////////////////////////////////////////////////////////////////////////
// CSceneManager

CSceneManager::CSceneManager() : CurrentScene(&GlobalScene)
{
	SetName("Scene manager");
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

void CFFPRenderer::PushModel(const CRenderConfig *Sender, const CModel * AModel)
{
	if (AModel == NULL)
		return;
	assert(Sender != NULL && AModel != NULL);
	CBetterVertexHolder *VertexHolder = &PrimitiveHolders[AModel->GetModelType() - 1];

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
			TexturedGeometry.push_back(new CBetterTextureVertexHolder()); // memory leak's here
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
			TempVector += Transformation.GetTranslation();//Sender->Position;
			if (!CRenderManager::Instance()->Transformator.doConsiderCamera)
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
		TempVector = (Vertex * Transformation.GetScaling() * Transformation.GetScaling());
 		if (!Equal(Transformation.GetAngle(), 0.0f))
 			TempVector *= Matrix2(DegToRad(-Transformation.GetAngle()));
		TempVector += Transformation.GetTranslation();// + Sender->GetPosition();//Sender->Position;
// 		if (!Sender->doIgnoreCamera)
// 			TempVector += CRenderManager::Instance()->Camera.GetTranslation();
		VertexHolder->PushVertex(Vector3(static_cast<int>(TempVector.x), static_cast<int>(TempVector.y), Transformation.GetDepth()), Sender->Color);
	}

}

void CFFPRenderer::Render()
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_POINTS);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	PrimitiveHolders[MODEL_TYPE_POINTS - 1].RenderPrimitive(GL_POINTS);
	glEnable(GL_BLEND);
	glEnable(GL_LINES);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	PrimitiveHolders[MODEL_TYPE_LINES - 1].RenderPrimitive(GL_LINES);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);	
	PrimitiveHolders[MODEL_TYPE_TRIANGLES - 1].RenderPrimitive(GL_TRIANGLES);

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

	Clear();
}

CFFPRenderer::~CFFPRenderer()
{

}

void CFFPRenderer::Clear()
{
	for(unsigned int i = 0; i < TOTAL_HOLDERS; i++)
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
	glColorPointer(4, GL_FLOAT, 0, Colors);
	glVertexPointer(3, GL_FLOAT, 0, Vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(Type, 0, VertexCount);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
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
	Scaling *= rhs.Scaling;
	Translation += rhs.Translation * Matrix2(DegToRad(-Rotation));
	Rotation = rhs.Rotation;
	return *this;
}

float CTransformation::GetAngle() const
{
	return Rotation;
}

float CTransformation::GetDepth() const
{
	return DepthOffset;
}

float CTransformation::GetScaling() const
{
	return Scaling;
}

const Vector2& CTransformation::GetTranslation() const
{
	return Translation;
}

Vector2& CTransformation::GetTranslation()
{
	return Translation;
}

void CTransformation::SetAngle(float Angle)
{
	Rotation = Angle;
}

void CTransformation::SetDepth(float ADepth)
{
	DepthOffset = ADepth;
}

void CTransformation::SetScaling(float AScaling)
{
	Scaling = AScaling;
}

void CTransformation::SetTranslation(const Vector2 &Position)
{
	Translation = Position;
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

void CTransformator::PushTransformation(const CTransformation &ATransformation)
{
	TransformationStack.push_back(CurrentTransformation);
	CurrentTransformation += ATransformation;
}

void CTransformator::PopTransformation()
{
	CurrentTransformation = TransformationStack.back();
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
// CFFPRenderer

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

	glColorPointer(4, GL_FLOAT, 0, Colors);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoords);
	glVertexPointer(3, GL_FLOAT, 0, Vertices);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(Type, 0, VertexCount);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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

/////////////////////////////////////////////////////////////////////////
// CModel

CModel::CModel(EModelType AModelType /*= MODEL_TYPE_NOT_A_MODEL*/, 
			   CTexture * ATexture /*= NULL*/, unsigned int AVerticesNumber /*= 0*/,
			   Vector2* AVertices /*= NULL*/, Vector2* ATexCoords /*= NULL*/) : 
	Texture(ATexture), ModelType(AModelType), Vertices(NULL),
	TexCoords(NULL), VerticesNumber(AVerticesNumber)
{
	if (VerticesNumber == 0)
		return;
	Vertices = new Vector2[VerticesNumber];
	std::copy(AVertices, AVertices + VerticesNumber, Vertices);
	if (ATexCoords == NULL)
		return;
	TexCoords = new Vector2[VerticesNumber];
	std::copy(ATexCoords, ATexCoords + VerticesNumber, TexCoords);
}

CModel::~CModel()
{
	SAFE_DELETE_ARRAY(Vertices);
	SAFE_DELETE_ARRAY(TexCoords);
}

void CModel::SetTexture(CTexture *ATexture)
{
	Texture = ATexture;
}

void CModel::SetModelType(EModelType AModelType)
{
	ModelType = AModelType;
}

CTexture* CModel::GetTexture() const
{
	return Texture;
}

const Vector2* CModel::GetVertices() const
{
	return Vertices;
}

const Vector2* CModel::GetTexCoords() const
{
	return TexCoords;
}

EModelType CModel::GetModelType() const
{
	return ModelType;
}

int CModel::GetVertexNumber() const
{
	return VerticesNumber;
}

