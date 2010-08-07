#include "2de_Engine.h"
#include "SoundCheck.h"

CEngine *Engine = CEngine::Instance();

CSoundCheck *SoundCheck = NULL;

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	void OnInitialize();

};

void CCustomStateHandler::OnInitialize()
{
	SoundCheck = CFactory::Instance()->New<CSoundCheck>("SoundCheck");
}

int main(int argc, char* argv[])
{
	Engine->SetStateHandler<CCustomStateHandler>();
	CEnvironment::Paths::SetConfigPath("Config/");
	Engine->SetState(CEngine::STATE_CONFIG_NAME, "SoundCheck.xml");
	Engine->Run();

	return 0;
}
