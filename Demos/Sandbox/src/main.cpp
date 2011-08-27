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
		Deku2d::EventManager->Subscribe("KeyDown", this);
	}

	void ProcessEvent(const Deku2d::CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown" && AEvent.GetData<Uint16>("Sym") == SDLK_SPACE)
		{
			if (Deku2d::GLWindow->GetFullscreen())
			{
				Deku2d::GLWindow->SetFullscreen(false);
				Deku2d::GLWindow->SetSize(640, 480);
				Deku2d::GLWindow->Initialize();
			}
			else
			{
				/*GLWindow->SetSize(2048, 1152);
				GLWindow->SetFullscreen(true);*/
 				Deku2d::GLWindow->SetVideoMode(Deku2d::GLWindow->GetDesktopVideoMode());
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
	Deku2d::CAbstractScene *NewScene = Deku2d::SceneManager->CreateScene();
	Deku2d::SceneManager->SetCurrentScene(NewScene);
	Deku2d::LuaVirtualMachine->RunScript(Deku2d::Factory->Get<Deku2d::CScript>(Deku2d::Config->Section("Data")["InitScript"]));
	Deku2d::CEngine::Instance()->RootGameObject->Attach(Deku2d::Factory->New<CTest>("SetSizeTest"));
	//SoundMixer->PlayMusic(MusicManager->GetMusicByName("Iggy"), 0, -1);
	Deku2d::SoundMixer->SetMusicVolume(128);
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
