#include "Engine.h"
#include "Game.h"
//#include "LuaUtils.h"
#include "Tanks.h"

CEngine* Ninja = CEngine::Instance();
//////////////////////////////////////////////////////////////////////////
CFont* Font;
int ScreenWidth = 0;
int ScreenHeight = 0;
Vector2 Position = V2Zero;
CParticleSystem *FontEffect;
float TimeOut = 0, TimeLimit = 2;
RGBAf FadeClr = RGBAf(1.0f, 1.0f, 1.0f ,1.0f);
//////////////////////////////////////////////////////////////////////////
CTankManager* Tanks;

#define SCALE_TITLE 2.0f
#define TITLE_TEXT "Tanks"

bool StartGame()
{
	Tanks = new CTankManager();
	return true;
}

class CTitleScreen : public CRenderObject
{
public:
	CTitleScreen()
	{
		Ninja->RenderManager.AddObject(this);
		name = "Title screen tanks";
	}
	~CTitleScreen()
	{
		
		StartGame();
	}
	bool Render()
	{
		glLoadIdentity();
		FadeClr.glSet();
		FontEffect->Render();
		Font->tClr = FadeClr;
		Font->Print(TITLE_TEXT);
		return true;
	}
};

CTitleScreen *TitleScreen;

bool Init()
{	
	Font = Ninja->FontManager->GetFont("Font");
	Ninja->FontManager->SetCurrentFont("Font");
	//////////////////////////////////////////////////////////////////////////
	Ninja->GetState(STATE_SCREEN_WIDTH, &ScreenWidth);
	Ninja->GetState(STATE_SCREEN_HEIGHT, &ScreenHeight);
	Position.x = ScreenWidth - Font->GetStringWidth(TITLE_TEXT)*SCALE_TITLE;
	Position.y = ScreenHeight - Font->GetStringHeight(TITLE_TEXT)*SCALE_TITLE;
	Position *= 0.5f;
	Font->Pos = Position;
	Font->scale.In(SCALE_TITLE, SCALE_TITLE);
	//////////////////////////////////////////////////////////////////////////
	FontEffect = new CParticleSystem;
	FontEffect->name = "Title bubbles";
	FontEffect->Init();
	FontEffect->Info.sc = RGBAf(0.5f, 0.5f, 0.5f, 1.0f);
	FontEffect->Info.ec = RGBAf(0.0f, 0.0f, 0.0f, 0.5f);
	FontEffect->visible = true;	
	Vector2 *pnts2 = new Vector2 [2];
	pnts2[1] = Position - Vector2(20, 0.0f);
	pnts2[0] = Position + Vector2(Font->GetStringWidth(TITLE_TEXT)*SCALE_TITLE, 0.0f);
	FontEffect->Info.plife = 1.0;
	FontEffect->Info.emission = 10;///*debug 1 */ 3000;
	FontEffect->Info.startsize = 4;
	// debug //
	FontEffect->Info.sizevar = 2.0f;
	FontEffect->Info.p = Vector2(0, 0);


	//FontEffect->Texture = Ninja->TextureManager->GetTextureByName("Particle");
	FontEffect->SetGeometry(pnts2, 2);


	GuiManager.visible = false;
	TitleScreen = new CTitleScreen;
	Ninja->RenderManager.DelObject("Title screen tanks");
	SAFE_DELETE(TitleScreen);
	return true;
}

bool UpdateTitle(scalar dt)
{
	TimeOut += dt;
	FadeClr -= RGBAf(0.01, 0.01, 0.01, 0.01);
	if (TimeOut >= TimeLimit)
	{
		Ninja->RenderManager.DelObject("Title screen tanks");
		SAFE_DELETE(TitleScreen);
	}
	FontEffect->Update(dt);
	return true;
}

int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "Tanks.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	//Ninja->SetState(STATE_UPDATE_FUNC, &UpdateTitle);	
	Ninja->Run();
	Ninja->FreeInst();
	return 0x1;
}
