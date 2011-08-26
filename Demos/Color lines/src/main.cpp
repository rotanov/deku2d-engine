#include "2de_Engine.h"
#include "Color Lines.h"
using namespace Deku2d;

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
};

bool CCustomStateHandler::OnInitialize()
{
	CGLWindow::Instance()->SetBackgroundColor(RGBAf(0.2f, 0.2f, 0.2f, 1.0f));
 	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
 	CSceneManager::Instance()->SetCurrentScene(NewScene);
//	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>(CConfig::Instance()->Section("Data")["InitScript"]));
	CGridGame * GridGame = CFactory::Instance()->New<CGridGame>("GridGame");
	return true;
}

int main(int argc, char *argv[])
{
	CEngine *Engine = CEngine::Instance();
	Engine->SetProgramName("Color lines");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run(argc, argv);
	return EXIT_SUCCESS;
}