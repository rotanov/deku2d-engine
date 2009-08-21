#include "Engine.h"
#include "LuaUtils.h"
#include "OpenglUtils.h"
#include "GameUtils.h"

// Константы интерфейса.
const int				INTERFACE_OFFSET_X = 128;
const int				LEFT_MARGIN = 8;
const int				EDIT_WIDTH = INTERFACE_OFFSET_X - LEFT_MARGIN*3;
const int				EDIT_HEIGHT = 32;
const int				BUTTON_WIDTH = INTERFACE_OFFSET_X - LEFT_MARGIN*3;
const int				BUTTON_HEIGHT = 32;

CEngine*				Ninja = CEngine::Instance();
byte					Zoom  = 2;
CTexture*				FontTexture = NULL;
CFont*					Font = NULL;
char					CurrentSymbol = 0;
int						OffsetX = INTERFACE_OFFSET_X + 5;
int						OffsetY = 0.0f;

CEditMini*				edFontTextureName = NULL;
CEditMini*				edFontname = NULL;
CEditMini*				edCurrentCymbol = NULL;
CEditMini*				edSampleText = NULL;

class CInput : public CObject
{
public:
	CInput()
	{
		Ninja->AddKeyInputFunction(&CObject::InputHandling, this);
	}
	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
	{
		if (state == KEY_PRESSED)
		{
			switch(key)
			{
				case 
			}
		}
		return true;
	}
} Input;

bool LoadFont()
{
	Font = Ninja->FontManager->GetFont(edFontname->text.c_str());
	if (!Font)
	{
		MessageBox(0, "Font not found", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	return true;
}

bool SaveFont()
{
	return true;
}

bool LoadTexture()
{
	FontTexture = Ninja->TextureManager->GetTextureByName(edFontTextureName->text);
	if (!FontTexture)
	{
		MessageBox(0, "Texture not found", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	if (Font)
		Font->AssignTexture(FontTexture);
	return true;
}

bool Init()
{
	CButtonMini *btn = new CButtonMini(CAABB(LEFT_MARGIN, 20, BUTTON_WIDTH, BUTTON_HEIGHT), "Load font", RGBAf(0.4f, 0.4f, 0.4f, 1.0f), LoadFont);	
				 btn = new CButtonMini(CAABB(LEFT_MARGIN, 100, BUTTON_WIDTH, BUTTON_HEIGHT), "Save font", RGBAf(0.5f, 0.5f, 0.6f, 1.0f), NULL);
				 btn = new CButtonMini(CAABB(LEFT_MARGIN, 200, BUTTON_WIDTH, BUTTON_HEIGHT), "Load texture", RGBAf(0.6f, 0.7f, 0.8f, 1.0f), LoadTexture);
				 btn = new CButtonMini(CAABB(LEFT_MARGIN, 300, BUTTON_WIDTH, BUTTON_HEIGHT), "Fuck yourself", RGBAf(0.9f, 0.8f, 0.2f, 1.0f), NULL);

	edFontTextureName = new CEditMini();
	edFontTextureName->aabb = CAABB(LEFT_MARGIN, 350, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontTextureName->text = "Font";
	edFontTextureName->color = btn->color;

	edFontname = new CEditMini();
	edFontname->aabb = CAABB(LEFT_MARGIN, 500, EDIT_WIDTH, BUTTON_HEIGHT);
	edFontname->text = "Font";
	edFontname->color = btn->color;

	return true;
}

//glColor3f(.4f, .4f, .4f);
//glColor3f(.5f, .5f, .6f);
//glColor3f(0.6f, 0.7f, 0.8f);
//glColor3f(0.9f, 0.8f, 0.2f);



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
	Ninja->FreeInst();
	return 0x01;
}