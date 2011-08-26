#include "2de_Engine.h"
using namespace Deku2d;

CEngine *Engine = CEngine::Instance();

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
};

bool CCustomStateHandler::OnInitialize()
{
	// do something..
	// i put it here just to show how it works and as placeholder for user's code..

	return true;
}

int main(int argc, char *argv[])
{
	Engine->SetProgramName("Demo template");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run(argc, argv);
	return 0;
}



