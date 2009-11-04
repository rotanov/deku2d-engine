#include "2de_Engine.h"
#include "2de_LuaUtils.h"
#include "2de_GameUtils.h"

// ���������, ������������ ������������ ��������� ��������� ����������.
const int				INTERFACE_OFFSET_X	= 128;
const int				LEFT_MARGIN			= 8;
const int				EDIT_WIDTH			= INTERFACE_OFFSET_X - LEFT_MARGIN*3;
const int				EDIT_HEIGHT			= 32;
const int				BUTTON_WIDTH		= INTERFACE_OFFSET_X - LEFT_MARGIN*3;
const int				BUTTON_HEIGHT		= 32;
// ���������, ������������ ���� ��������� ���������� ����� ���. ���� ������.
const RGBAf				COLOR_FIRST(.4f, .4f, .4f, 1.0f);
const RGBAf				COLOR_SECOND(.5f, .5f, .6f, 1.0f);
const RGBAf				COLOR_THIRD(0.6f, 0.7f, 0.8f, 0.5f);
const RGBAf				COLOR_FOURTH(0.9f, 0.8f, 0.2f, 1.0f);
// ������������ ����������.
CEngine					*Ninja				= CEngine::Instance();
byte					Zoom				= 2;
CTexture				*FontTexture		= NULL;
CFont					*Font				= NULL;
char					CurrentSymbol		= 0;
int						OffsetX				= INTERFACE_OFFSET_X + 5;
int						OffsetY				= 0.0f;

CEdit					*edFontTextureName	= NULL;
CEdit					*edFontname			= NULL;
CEdit					*edCurrentCymbol	= NULL;
CEdit					*edSampleText		= NULL;

// �� ����� ���� ��� �� �� ����� Load() � ������ Acquire() ��� ��� ��������� ��� ������ ������ ��� ��������������.
// �.�. ��� ������������ ������� ��� ����� Load, � ��� ���, ��� �������� ������ ���� ����������� - ��� Asquire()
bool LoadFont()  
{
	Font = Ninja->FontManager->GetFont(edFontname->text.c_str());
	if (Font == NULL)
	{
		MessageBox(0, "Font not found", "Error", MB_ICONERROR | MB_OK);  // ��� ��������� �������.
		return false;
	}
	FontTexture = Font->GetTexture();
	return true;
}

bool SaveFont()
{
	// �������� ��� ���������� ���������, �������� � ������, � ����� �����.
	return true;
}

bool LoadTexture()  // ����� �� �� Load() � Acquire().
{
	FontTexture = Ninja->TextureManager->GetTextureByName(edFontTextureName->text);
	if (FontTexture == NULL)
	{
		MessageBox(0, "Texture not found", "Error", MB_ICONERROR | MB_OK);
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
		MousePosition = Vector2(x, wheight - y);

		if (isGripToolEnabled &&  ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))))
		{
			OffsetX += MouseDelta.x;
			OffsetY -= MouseDelta.y;
		}
		Sleep(1);
		return true;
	}
};

CFontEditor *FontEditor;

bool Init()
{	
	SDL_ShowCursor(0);
	
	(new CButton(CAABB(LEFT_MARGIN, 20, BUTTON_WIDTH, BUTTON_HEIGHT), "Load font", RGBAf(0.4f, 0.4f, 0.4f, 1.0f), LoadFont))->SetParent(&GuiManager);	
	(new CButton(CAABB(LEFT_MARGIN, 200, BUTTON_WIDTH, BUTTON_HEIGHT), "Load texture", RGBAf(0.6f, 0.7f, 0.8f, 1.0f), LoadTexture))->SetParent(&GuiManager);

	edFontTextureName = new CEdit();
	edFontTextureName->aabb = CAABB(LEFT_MARGIN, 350, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontTextureName->text = "Font";
	edFontTextureName->color = RGBAf(0.5f, 0.5f, 0.6f, 0.9f);
	edFontTextureName->SetParent(&GuiManager);

	edFontname = new CEdit();
	edFontname->aabb = CAABB(LEFT_MARGIN, 500, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontname->text = "Font";
	edFontname->color = RGBAf(0.8f, 0.3f, 0.5f, 0.9f);
	edFontname->SetParent(&GuiManager);

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