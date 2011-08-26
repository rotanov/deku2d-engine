#include "2de_Engine.h"

#define ENGINE Deku2d::CEngine::Instance()
#define RENDER Deku2d::CRenderManager::Instance()
Deku2d::CEngine* Ninja = ENGINE;

/*
��� �������� � ��� ���� ����� DraggeablePrototype;
�� ������ ������������ ���������� ����������� �������� ���� ���������������� �����.
[Root]->[Ball]->[Draggeabl] ��� [Root]->[Draggeable]->[Ball] 
� �� ��� ������ �������� ��� ������������� �������������� ������ ���� ��������� ��� ��������, ����� ������ �� ����.
� ����� ������, �� ����� ����� � �������� �������, � �������� ��� �����������. ���� ����� �����, ��� ������� ����� �� �������� ���� � �� ����� ��� �����������/�������� ������ � ��������. ����� �� ������ ������� ��������� ���� "����� ��� ����, � ���� ���� �����������, ������, ��� ������ ���� ComponentRoot � ������� ������� ������ PlaceableComponent'� ��� ��� ���� �� ���� ���������� (��� ������������� �� �������-��)"

��� ������ ���� ������ �������� ������ �������.
���� ����� Message passing ���-�� ��������� ������ ���������.
+ ��������� ���� "�� �����, ��� � ������� � ��������" ��������� ���-�� ���������.
���, �� ��� ��� ��-�� �������, � � � ���������� ��� ���������, � ����

*/

class CTest : public Deku2d::CGameObject
{
public:
	CTest()
	{
		Deku2d::CEventManager::Instance()->Subscribe("KeyDown", this);
	}

	void ProcessEvent(const Deku2d::CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown" && AEvent.GetData<Uint16>("Sym") == SDLK_SPACE)
		{
			if (Deku2d::CGLWindow::Instance()->GetFullscreen())
			{
				Deku2d::CGLWindow::Instance()->SetFullscreen(false);
				Deku2d::CGLWindow::Instance()->SetSize(640, 480);
				Deku2d::CGLWindow::Instance()->Initialize();
			}
			else
			{
				/*CGLWindow::Instance()->SetSize(2048, 1152);
				CGLWindow::Instance()->SetFullscreen(true);*/
 				Deku2d::CGLWindow::Instance()->SetVideoMode(Deku2d::CGLWindow::Instance()->GetDesktopVideoMode());
			}
		}
	}

};

class CCustomStateHandler : public Deku2d::CAbstractStateHandler
{
public:
	bool OnInitialize();
	void OnFinalize();
};

bool CCustomStateHandler::OnInitialize()
{
	Deku2d::CAbstractScene *NewScene = Deku2d::CSceneManager::Instance()->CreateScene();
	Deku2d::CSceneManager::Instance()->SetCurrentScene(NewScene);
	Deku2d::CLuaVirtualMachine::Instance()->RunScript(Deku2d::CFactory::Instance()->Get<Deku2d::CScript>(Deku2d::CConfig::Instance()->Section("Data")["InitScript"]));
	Deku2d::CEngine::Instance()->RootGameObject->Attach(Deku2d::CFactory::Instance()->New<CTest>("SetSizeTest"));
	//CSoundMixer::Instance()->PlayMusic(CMusicManager::Instance()->GetMusicByName("Iggy"), 0, -1);
	Deku2d::CSoundMixer::Instance()->SetMusicVolume(128);
	return true;
}

void CCustomStateHandler::OnFinalize()
{
	// wait...what?
}

int main(int argc, char *argv[])
{
	Ninja->SetProgramName("Sandbox");
	Ninja->SetStateHandler<CCustomStateHandler>();
	Ninja->Run(argc, argv);
	return EXIT_SUCCESS;
}
