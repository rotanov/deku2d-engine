#include "2de_Engine.h"
#include "Color Lines.h"
using namespace Deku2D;

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
};

bool CCustomStateHandler::OnInitialize()
{
	GLWindow->SetBackgroundColor(RGBAf(0.2f, 0.2f, 0.2f, 1.0f));
 	CAbstractScene *NewScene = SceneManager->CreateScene();
 	SceneManager->SetCurrentScene(NewScene);
//	LuaVirtualMachine->RunScript(Factory->Get<CScript>(Config->Section("Data")["InitScript"]));
	CGridGame * GridGame = Factory->New<CGridGame>("GridGame");
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
