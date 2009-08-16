#include "Engine.h"
#include "Game.h"
#include "LuaUtils.h"

CEngine* Ninja = CEngine::Instance();

class CHaha : public CRenderObject
{
	
};

CHaha tmp;

bool Init()
{	
	Ninja->FontManager->SetCurrentFont("Font");
	RGBAf bgclr = RGBAf(0.8f, 0.2f, 0.3f, 0.0f);
	Ninja->SetState(STATE_GL_BG_COLOR, (void*)&bgclr);

	tmp.name = "aaa";
	return true;
}

bool Draw()
{	
	glLoadIdentity();
	tmp.Render();
	DWORD FPS;
	Ninja->GetState(STATE_FPS_COUNT, &FPS);
	Ninja->FontManager->PrintEx(10, 10, 1, "Fps: %d", FPS);
	return true;
}

int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_CONFIG_NAME, "Sandbox.xml");
	Ninja->SetState(STATE_USER_INIT_FUNC, &Init);
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);	
	Ninja->Run();
	Ninja->FreeInst();
	return 0x1;
}
