#include "2de_Engine.h"
#include "SoundCheck.h"

CEngine *Engine = CEngine::Instance();

CSoundCheck *SoundCheck = NULL;

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();

};

bool CCustomStateHandler::OnInitialize()
{
	SoundCheck = CFactory::Instance()->New<CSoundCheck>("SoundCheck");
	return true;
}

int main(int argc, char* argv[])
{
	Engine->SetStateHandler<CCustomStateHandler>();
	CEnvironment::Paths::SetConfigPath("Config/");
	Engine->SetProgramName("SoundCheck");
	Engine->Run();

	return 0;
}
