#include "2de_Engine.h"
#include "2de_GameUtils.h"
#include "Pacman.h"
#include "2de_Gui.h"

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();

};

void CCustomStateHandler::OnInitialize()
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

	CFactory::Instance()->Add(new CPacmanGame(CFactory::Instance()->New<CPacmanPlayer>("CPacmanPlayer")), "CPacmanGame");
	CFactory::Instance()->Get<CFont>("Font")->LoadFromFile();	// ???
	CSoundMixer::Instance()->PlayMusic(CMusicManager::Instance()->GetMusicByName("PacMan"), 5000);
}

int main(int argc, char *argv[])
{
	CEngine	*Ninja = CEngine::Instance();
	CEnvironment::Paths::SetConfigPath("Config/");
	Ninja->SetState(CEngine::STATE_CONFIG_NAME, "Pac man.xml");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run();
	return 0;
}
