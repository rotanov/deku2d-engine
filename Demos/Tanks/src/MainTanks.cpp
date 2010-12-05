#include "2de_Engine.h"

//	There should be (and acttually ever was) demo of Battle city based game

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
	Engine->SetProgramName("Tanks");
	Engine->SetStateHandler<CCustomStateHandler>();
	Engine->Run(argc, argv);
	return 0;
}