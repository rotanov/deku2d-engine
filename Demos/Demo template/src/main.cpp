#include "2de_Engine.h"

CEngine *Engine = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();
};

void CCustomStateHandler::OnInitialize()
{
	// do something..
	// i put it here just to show how it works and as placeholder for user's code..
}

int main(int argc, char *argv[])
{
	CEnvironment::Paths::SetConfigPath("Config/");
	Engine->SetProgramName("Demo template");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run();
	return 0;
}
