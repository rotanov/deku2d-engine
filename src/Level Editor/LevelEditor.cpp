#include "Ninja.h"
#include "Game.h"

CNinja* Ninja = CNinja::Instance();

bool Init()
{	

	Ninja->RenderManager.SortByAlpha();
	Ninja->RenderManager.SortByZ();




	Ninja->FontManager->SetCurrentFont("Font");

	return true;
}

bool Draw()
{	
	glLoadIdentity();
	Ninja->FontManager->PrintEx(560, 460, 1, "Fps: %d", Ninja->GetFps());


	return true;
}
int	main(int argc, char *argv[])
{
	Ninja->SetState(STATE_RENDER_FUNC, &Draw);
	Ninja->SetState(STATE_USER_INIT, &Init);

	Ninja->Run();
	Ninja->FreeInst();

	return 0x0;
}
