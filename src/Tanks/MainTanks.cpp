#include "Engine.h"
#include "Game.h"
//#include "LuaUtils.h"
#include "Tanks.h"

CEngine* Ninja = CEngine::Instance();
//////////////////////////////////////////////////////////////////////////
CFont* Font;
int ScreenWidth = 0;
int ScreenHeight = 0;
Vector2 fPosition = V2Zero;
CParticleSystem *FontEffect;
bool begintimeout = false;
float TimeOut = 0, TimeLimit = 2;
RGBAf FadeClr = RGBAf(1.0f, 1.0f, 1.0f ,1.0f);
//////////////////////////////////////////////////////////////////////////
CTankManager* Tanks;

#define SCALE_TITLE 4.0f
#define TITLE_TEXT "Tanks"

class CTitleScreen : public CRenderObject, public CUpdateObject
{
public:
	CTitleScreen()
	{
		Ninja->RenderManager.AddObject(this);
		Ninja->UpdateManager.AddObject(this);
		name = "Title screen tanks";
	}
	~CTitleScreen()
	{		
	}
	bool Render()
	{
		glLoadIdentity();
		Font->Pos = fPosition;
		Font->scale.In(SCALE_TITLE, SCALE_TITLE);

		FadeClr.glSet();
		FontEffect->Info.sc = FadeClr;
		FontEffect->Render();
		Font->tClr = FadeClr;
		Font->Print(TITLE_TEXT);
		return true;
	}
	bool Update(scalar dt)
	{
		if (begintimeout)
		{
			TimeOut += dt;
			FadeClr -= RGBAf(0.01, 0.01, 0.01, 0.01);
			if (TimeOut >= TimeLimit)
			{
				Ninja->RenderManager.DelObject("Title screen tanks");
				Ninja->UpdateManager.DelObject("Title screen tanks");
				dynamic_cast<CRenderObject*>(GuiManager.GetObject("Root menu item"))->visible = false;
				Tanks = new CTankManager;
			}
		}
		FontEffect->Update(dt);
		return true;
	}
};

bool StartGame()
{
	begintimeout = true;
	return true;
}

bool EndGame()
{
	SDLGLExit(1);
	return true;
}
bool Options()
{
	FadeClr = RGBAf(0.3f, 0.5f, 0.4f, 1.0f);
	return true;
}

CTitleScreen *TitleScreen;

bool Init()
{	
	Font = Ninja->FontManager->GetFont("Font");
	Ninja->FontManager->SetCurrentFont("Font");
	//////////////////////////////////////////////////////////////////////////
	Ninja->GetState(STATE_SCREEN_WIDTH, &ScreenWidth);
	Ninja->GetState(STATE_SCREEN_HEIGHT, &ScreenHeight);
	fPosition.x = ScreenWidth - Font->GetStringWidth(TITLE_TEXT)*SCALE_TITLE;
	fPosition.y = ScreenHeight - Font->GetStringHeight(TITLE_TEXT)*SCALE_TITLE;
	fPosition *= 0.5f;

	//////////////////////////////////////////////////////////////////////////
	FontEffect = new CParticleSystem;
	FontEffect->name = "Title bubbles";
	FontEffect->Init();
	FontEffect->Info.sc = RGBAf(0.5f, 0.5f, 0.5f, 1.0f);
	FontEffect->Info.ec = RGBAf(0.0f, 0.0f, 0.0f, 0.5f);
	FontEffect->visible = true;	
	Vector2 *pnts2 = new Vector2 [2];
	pnts2[1] = fPosition + Vector2(0.0f, 20.0f);
	pnts2[0] = fPosition + Vector2(Font->GetStringWidth(TITLE_TEXT)*SCALE_TITLE, 20.0f);
	FontEffect->Info.plife = 1.0;
	FontEffect->Info.emission = 10;///*debug 1 */ 3000;
	FontEffect->Info.startsize = 10;
	// debug //
	FontEffect->Info.sizevar = 2.0f;
	FontEffect->Info.p = Vector2(0, 0);


	//FontEffect->Texture = Ninja->TextureManager->GetTextureByName("Particle");
	FontEffect->SetGeometry(pnts2, 2);


	GuiManager.visible = false;
	TitleScreen = new CTitleScreen;
	
	CMenuItem *MenuRoot = new CMenuItem;
	MenuRoot->name = "Root menu item";
	MenuRoot->text = "Root";

	CMenuItem *next = new CMenuItem();
	next->name = "menu item Start";
	next->text = "Start";
	next->CallProc = &StartGame;
	next->Position = Vector2(ScreenWidth*0.5f - 20,180);
	MenuRoot->AddObject(next);

	next = new CMenuItem();
	next->name = "menu item Options";
	next->text = "Options";
	next->Position = Vector2(ScreenWidth*0.5f - 20,160);
	next->CallProc = &Options;
	MenuRoot->AddObject(next);

	next = new CMenuItem();
	next->name = "menu item Exit";
	next->text = "Exit";
	next->Position = Vector2(ScreenWidth*0.5f - 20,140);
	next->CallProc = &EndGame;
	MenuRoot->AddObject(next);

	dynamic_cast<CRenderObject*>(GuiManager.GetObject("Root menu item"))->visible = true;
	GuiManager.SetFocus(MenuRoot);
	
	return true;
}


int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "Tanks.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	Ninja->FreeInst();
	return 0x1;
}
