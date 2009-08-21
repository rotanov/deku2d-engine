#include "Engine.h"
#include "Game.h"
//#include "LuaUtils.h"
#include "Tanks.h"

CEngine* Ninja = CEngine::Instance();
int ScreenWidth = 0, ScreenHeight = 0;
Vector2 Position = V2Zero;
CTankManager* Tanks;
CFont* Font;

#define SCALE_TITLE 2.0f
#define TITLE_TEXT "Tanks"


bool Init()
{	
	Font = Ninja->FontManager->GetFont("Font");
	Ninja->GetState(STATE_SCREEN_WIDTH, &ScreenWidth);
	Ninja->GetState(STATE_SCREEN_HEIGHT, &ScreenHeight);
	Position.x = ScreenWidth - Font->GetStringWidth(TITLE_TEXT)*SCALE_TITLE;
	Position.y = ScreenHeight - Font->GetStringHeight(TITLE_TEXT)*SCALE_TITLE;
	Position *= 0.5f;
	return true;
}

bool StartGame()
{
	Tanks = new CTankManager();
	return true;
}

bool Draw()
{	
	glLoadIdentity();
	DWORD FPS;
	Ninja->GetState(STATE_FPS_COUNT, &FPS);
	//Font->PrintEx(10, 10, 1, "Fps: %d", FPS);
	return true;
}

bool DrawTitle()
{
	glLoadIdentity();
	Font->Pos = Position;
	Font->scale.In(SCALE_TITLE, SCALE_TITLE);
	Font->Print(TITLE_TEXT);
	return true;
}

int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(STATE_RENDER_FUNC, &DrawTitle);	
	Ninja->Run();
	Ninja->FreeInst();
	return 0x1;
}
