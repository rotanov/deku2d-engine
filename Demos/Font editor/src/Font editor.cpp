#include "2de_Engine.h"
#include "2de_LuaUtils.h"
#include "2de_GameUtils.h"

// Константы, определяющие расположение некоторых элементов интерфейса.
const int				INTERFACE_OFFSET_X	= 128;
const int				LEFT_MARGIN			= 8;
const int				EDIT_WIDTH			= INTERFACE_OFFSET_X - LEFT_MARGIN*3;
const int				EDIT_HEIGHT			= 32;
const int				BUTTON_WIDTH		= INTERFACE_OFFSET_X - LEFT_MARGIN*3;
const int				BUTTON_HEIGHT		= 32;
// Константы, определяющие цвет элементов интерфейса.
const RGBAf				COLOR_FIRST(.4f, .4f, .4f, 1.0f);
const RGBAf				COLOR_SECOND(.5f, .5f, .6f, 1.0f);
const RGBAf				COLOR_THIRD(0.6f, 0.7f, 0.8f, 0.5f);
const RGBAf				COLOR_FOURTH(0.9f, 0.8f, 0.2f, 1.0f);
// Ограничения:
const float ZOOM_MAX = 16.0f;
const float ZOOM_MIN = 0.5f;
const float ZOOM_STEP = 1.0f;
// Всевозможные переменные.
CEngine					*Ninja				= CEngine::Instance();
float					Zoom				= 2.0f;
CTexture				*FontTexture		= NULL;
CFont					*Font				= NULL;
char					CurrentSymbol		= 0;
int						OffsetX				= INTERFACE_OFFSET_X + 5;
int						OffsetY				= 0.0f;

CEdit					*edFontTextureName	= NULL;
CEdit					*edFontname			= NULL;
CEdit					*edCurrentCymbol	= NULL;
CEdit					*edSampleText		= NULL;

// На самом деле это не то чтобы Load() а скорее Acquire() ибо все доступные нам шрифты движок уже проиндекировал.
// Т.е. для пользователя утилыты это будет Load, а для тех, кто способен читать этот комментарий - это Asquire()
bool LoadFont()  
{
	Font = Ninja->FontManager->GetFont(edFontname->Text.c_str());
	if (Font == NULL)
	{
		Log.Log("Error", "Font %s not found within data/fonts", edFontname->Text);
		return false;
	}
	FontTexture = Font->GetTexture();
	return true;
}

bool SaveFont()
{
	// Написать код сохранения изменений, сделаных в шрифте, в файле здесь.
	return true;
}

bool LoadTexture()  // Опять же не Load() а Acquire().
{
	FontTexture = Ninja->TextureManager->GetTextureByName(edFontTextureName->Text);
	if (FontTexture == NULL)
	{
#ifdef _WIN32
		MessageBox(0, "Texture not found", "Error", MB_ICONERROR | MB_OK);
#endif
		return false;
	}
	return true;
}

class CFontEditor : public CRenderObject, public CUpdateObject
{
public:
	Vector2 MousePosition;
	Vector2 MouseDelta;
	bool isGripToolEnabled;
	bool doGripping;
	int wheight, wwidth;
	void SetZoom(float AZoom)
	{
		float OldZoom = Zoom;
		Zoom = AZoom;
		Zoom = clampf(Zoom, ZOOM_MIN, ZOOM_MAX);
		if (Zoom == OldZoom)
			return;
		float TempX = (MousePosition.x - OffsetX) / (FontTexture->width * OldZoom);
		float TempY = (MousePosition.y - OffsetY) / (FontTexture->height * OldZoom);
		TempX = clampf(TempX, 0.0f, 1.0f);
		TempY = clampf(TempY, 0.0f, 1.0f);
		OffsetX = - TempX * (FontTexture->width * Zoom) + MousePosition.x;
		OffsetY = - TempY * (FontTexture->height * Zoom) + MousePosition.y;

	}
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
	{
		switch (state)
		{
		case KEY_PRESSED:
			switch(key)
			{
			case SDLK_g:
				isGripToolEnabled = true;
				break;
			case SDL_BUTTON_WHEELUP:
				SetZoom(Zoom + ZOOM_STEP);
				break;
			case SDL_BUTTON_WHEELDOWN:
				SetZoom(Zoom - ZOOM_STEP);
				break;
			}
		break;
		case KEY_RELEASED:
			switch(key)
			{
			case SDLK_g:
				//doGripping = false;
				break;
			}
			break;
		}
		return true;
	}

	CFontEditor()
	{
		isGripToolEnabled = false;
		doGripping = false;
		CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
	}

	bool Render()
	{

		Ninja->GetState(STATE_SCREEN_HEIGHT, &wheight);
		Ninja->GetState(STATE_SCREEN_WIDTH, &wwidth);

		CPrimitiveRender PRender;
		PRender.lClr = COLOR_FIRST;
		PRender.sClr = COLOR_THIRD;
		PRender.pClr = COLOR_FIRST;
		glLoadIdentity();
		PRender.grRectC(Vector2(.0f, .0f), Vector2(INTERFACE_OFFSET_X, wheight));
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);		
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);

		PRender.grCircleL(MousePosition, 5);
		int fps;
		Ninja->GetState(STATE_FPS_COUNT, &fps);
		Ninja->FontManager->PrintEx(10, 400, 0.0f, "FPS: %d", fps);
		glLoadIdentity();

		gToggleScissor(true);
		gScissor(INTERFACE_OFFSET_X, 0, wwidth - INTERFACE_OFFSET_X, wheight);
		glLoadIdentity();
		glTranslatef(OffsetX, OffsetY, .0f);
		glScalef(Zoom, Zoom, 1.0f);
		if(FontTexture)
		{
			FontTexture->Bind();
			RGBAf(1.0f, 1.0f, 1.0f, 1.0f).glSet();
			glBegin(GL_QUADS);
			glTexCoord2f(.0f, .0f);
			glVertex2f(.0f, .0f);

			glTexCoord2f(1.0f, .0f);
			glVertex2f(FontTexture->width, .0f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(FontTexture->width, FontTexture->height);

			glTexCoord2f(.0f, 1.0f);
			glVertex2f(.0f, FontTexture->height);
			glEnd();
		}
		
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LINE_WIDTH);
		glLineWidth(1.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if (Font)
		{
			glBegin(GL_LINES);
			for(int i=0;i<256;i++)
			{
				glVertex2i(Font->bbox[i].x0, Font->bbox[i].y0);
				glVertex2i(Font->bbox[i].x1, Font->bbox[i].y0);

				glVertex2i(Font->bbox[i].x1, Font->bbox[i].y0);
				glVertex2i(Font->bbox[i].x1, Font->bbox[i].y1);

				glVertex2i(Font->bbox[i].x1, Font->bbox[i].y1);
				glVertex2i(Font->bbox[i].x0, Font->bbox[i].y1);

				glVertex2i(Font->bbox[i].x0, Font->bbox[i].y1);
				glVertex2i(Font->bbox[i].x0, Font->bbox[i].y0);
			}
			glEnd();
		}
		gToggleScissor(false);
		return true;

	}
	
	bool Update(float dt)
	{
		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		MouseDelta = Vector2(x, y);
		SDL_GetMouseState(&x, &y);
		Ninja->GetState(STATE_MOUSE_XY, &MousePosition);
		//MousePosition = Vector2(x, wheight - y);

		if (isGripToolEnabled && !CAABB(0, 0, INTERFACE_OFFSET_X, wheight).Inside(MousePosition) &&  ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))))
		{
			OffsetX += MouseDelta.x;
			OffsetY -= MouseDelta.y;
		}
		
		return true;
	}
};

CFontEditor *FontEditor;

bool ExitFontEditor()
{
	SDLGLExit(0);
	return true;
}

bool Init()
{	
	SDL_ShowCursor(1);
#define BUTTONS_COUNT 3
	const char* ButtonNames[BUTTONS_COUNT] = {"Load font", "Load texture", "Exit", };
	const Callback ButtonCallers[BUTTONS_COUNT] = {&LoadFont, &LoadTexture, &ExitFontEditor, };
	
	for(int i = 0; i < BUTTONS_COUNT; i++)
		(new CButton(CAABB(LEFT_MARGIN, 20 + (BUTTON_HEIGHT+10)*i, BUTTON_WIDTH, BUTTON_HEIGHT), ButtonNames[i], RGBAf(0.4f, 0.4f, 0.4f, 1.0f), ButtonCallers[i]))->SetParent(&GuiManager);	
	

	edFontTextureName = new CEdit();
	edFontTextureName->SetParent(&GuiManager);
	edFontTextureName->aabb = CAABB(LEFT_MARGIN, 350, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontTextureName->Text = "Font";
	edFontTextureName->color = RGBAf(0.5f, 0.5f, 0.6f, 0.9f);
	

	edFontname = new CEdit();
	edFontname->SetParent(&GuiManager);
	edFontname->aabb = CAABB(LEFT_MARGIN, 500, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontname->Text = "Font";
	edFontname->color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);
	

	FontEditor = new CFontEditor;
	return true;
}

int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "FontEditor.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0x00;
}
