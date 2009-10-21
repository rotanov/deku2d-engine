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
// Константы, определяющие цвет элементов интерфейса будут тут. Чуть погодя.
const RGBAf				COLOR_FIRST(.4f, .4f, .4f, 1.0f);
const RGBAf				COLOR_SECOND(.5f, .5f, .6f, 1.0f);
const RGBAf				COLOR_THIRD(0.6f, 0.7f, 0.8f, 1.0f);
const RGBAf				COLOR_FOURTH(0.9f, 0.8f, 0.2f, 1.0f);
// Всевозможные переменные.
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

// На самом деле это не то чтобы Load() а скорее Acquire() ибо все доступные нам шрифты движок уже проиндекировал.
// Т.е. для пользователя утилыты это будет Load, а для тех, кто способен читать этот комментарий - это Asquire()
bool LoadFont()  
{
	Font = Ninja->FontManager->GetFont(edFontname->text.c_str());
	if (Font == NULL)
	{
		MessageBox(0, "Font not found", "Error", MB_ICONERROR | MB_OK);  // Это временное решение.
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
	FontTexture = Ninja->TextureManager->GetTextureByName(edFontTextureName->text);
	if (FontTexture == NULL)
	{
		MessageBox(0, "Texture not found", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	return true;
}

bool Init()
{
	(new CButton(CAABB(LEFT_MARGIN, 20, BUTTON_WIDTH, BUTTON_HEIGHT), "Load font", RGBAf(0.4f, 0.4f, 0.4f, 1.0f), LoadFont))->SetParent(&GuiManager);	
	(new CButton(CAABB(LEFT_MARGIN, 100, BUTTON_WIDTH, BUTTON_HEIGHT), "Save font", RGBAf(0.5f, 0.5f, 0.6f, 1.0f), NULL))->SetParent(&GuiManager);
	(new CButton(CAABB(LEFT_MARGIN, 200, BUTTON_WIDTH, BUTTON_HEIGHT), "Load texture", RGBAf(0.6f, 0.7f, 0.8f, 1.0f), LoadTexture))->SetParent(&GuiManager);
	(new CButton(CAABB(LEFT_MARGIN, 300, BUTTON_WIDTH, BUTTON_HEIGHT), "Fuck yourself", RGBAf(0.9f, 0.8f, 0.2f, 1.0f), NULL))->SetParent(&GuiManager);

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

	return true;
}

bool Draw()
{
	int wheight, wwidth;
	Ninja->GetState(STATE_SCREEN_HEIGHT, &wheight);
	Ninja->GetState(STATE_SCREEN_WIDTH, &wwidth);

	glLoadIdentity();
// 	CPrimitiveRender PRender;
// 	PRender.lClr = RGBAf(.4f, .4f, .4f, 1.0f);
// 	PRender.sClr = RGBAf(0.6f, 0.7f, 0.8f, 0.5f);
// 	PRender.pClr = RGBAf(.4f, .4f, .4f, 1.0f);
// 	PRender.	grRectC(Vector2(.0f, .0f), Vector2(INTERFACE_OFFSET_X, wheight));


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

int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "FontEditor.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	return 0x00;
}