#include "2de_Engine.h"

#define ENGINE Deku2D::CEngine::Instance()
#define RENDER Deku2D::CRenderManager::Instance()
Deku2D::CEngine* Ninja = ENGINE;

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

class CTest : public Deku2D::CGameObject
{
public:
	CTest()
	{
		Deku2D::EventManager->Subscribe("KeyDown", this);
	}

	void ProcessEvent(const Deku2D::CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown" && AEvent.GetData<Uint16>("Sym") == SDLK_SPACE)
		{
			if (Deku2D::GLWindow->GetFullscreen())
			{
				Deku2D::GLWindow->SetFullscreen(false);
				Deku2D::GLWindow->SetSize(640, 480);
				Deku2D::GLWindow->Initialize();
			}
			else
			{
				/*GLWindow->SetSize(2048, 1152);
				GLWindow->SetFullscreen(true);*/
 				Deku2D::GLWindow->SetVideoMode(Deku2D::GLWindow->GetDesktopVideoMode());
			}
		}
	}

};

class CCustomStateHandler : public Deku2D::CAbstractStateHandler
{
public:
	bool OnInitialize();
	void OnFinalize();
};

bool CCustomStateHandler::OnInitialize()
{
	Deku2D::CAbstractScene *NewScene = Deku2D::SceneManager->CreateScene();
	Deku2D::SceneManager->SetCurrentScene(NewScene);
	Deku2D::LuaVirtualMachine->RunScript(Deku2D::Factory->Get<Deku2D::CScript>(Deku2D::Config->Section("Data")["InitScript"]));
	Deku2D::CEngine::Instance()->RootGameObject->Attach(Deku2D::Factory->New<CTest>("SetSizeTest"));
	//SoundMixer->PlayMusic(MusicManager->GetMusicByName("Iggy"), 0, -1);
	Deku2D::SoundMixer->SetMusicVolume(128);
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
