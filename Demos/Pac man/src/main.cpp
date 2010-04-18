#include "2de_Engine.h"
#include "2de_GameUtils.h"
#include "Pacman.h"
#include "2de_Gui.h"

bool Init()
{	
// 	CMenuItem *MenuRoot = new CMenuItem(NULL, "Root menu item");
// 	MenuRoot->SetLayer(10);
// 	MenuRoot->SetFont(CFactory::Instance()->Get<CFont>("Font"));
// 	MenuRoot->SetPrimitiveRender(new CPrimitiveRender);
// 	(new CMenuItem(MenuRoot, "Play"))->Position = Vector2(320*0.5f - 20, 180);
// 	(new CMenuItem(MenuRoot, "Credits"))->Position = Vector2(320*0.5f - 20, 160);
// 	(new CMenuItem(MenuRoot, "Exit"))->Position = Vector2(320*0.5f - 20, 140);
// 	dynamic_cast<CRenderObject*>(CGUIManager::Instance()->GetObject("Root menu item"))->Visible = true;
// 	CGUIManager::Instance()->SetFocus(MenuRoot);

	new CPakmanGame(new CPacmanPlayer);
	CFactory::Instance()->Get<CFont>("Font")->LoadFromFile();	// ???
	CSoundMixer::Instance()->PlayMusic(CMusicManager::Instance()->GetMusicByName("PacMan"), 5000);
	return true;
}

int	main(int argc, char *argv[])
{
	CEngine	*Ninja = CEngine::Instance();
	Ninja->SetState(CEngine::STATE_CONFIG_PATH, "Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Pac man.xml");
	Ninja->SetState(CEngine::STATE_USER_INIT_FUNC, &Init);
	Ninja->Run();
	return 0x00;
}
