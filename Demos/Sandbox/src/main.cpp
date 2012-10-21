#include "2de_Engine.h"
#include "2de_TypeInfo.h"
#include "2de_Resource.h"

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
		else if (AEvent.GetName() == "KeyDown" && AEvent.GetData<Uint16>("Sym") == SDLK_d)
		{
			//Deku2D::SerializeObjectToXml(Deku2D::CEngine::Instance()->RootGameObject, "rootdump.xml");
			//Deku2D::SerializeObjectToJSON(Deku2D::CEngine::Instance()->RootGameObject, "rootdump.json");
			Deku2D::Serialization::ToJSON(Deku2D::CEngine::Instance()->RootGameObject, "CObject", "rootdump.json");
			//Deku2D::DeserializeObjectFromJSON("rootdump.json");
		}
	}

};

class CCustomStateHandler : public Deku2D::CAbstractStateHandler
{
public:
	bool OnInitialize();
	void OnFinalize();
};

class HRTimer
{
public:
	HRTimer();
	double GetFrequency();
	void StartTimer();
	double StopTimer();
private:
	LARGE_INTEGER start;
	LARGE_INTEGER stop;
	double frequency;
};

HRTimer::HRTimer(): start(), stop(),
frequency()
{
	frequency = this->GetFrequency();
}

double HRTimer::GetFrequency(void)
{
	LARGE_INTEGER proc_freq;
	QueryPerformanceFrequency(&proc_freq);
	return proc_freq.QuadPart;
}

void HRTimer::StartTimer(void)
{
	DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
	::QueryPerformanceCounter(&start);
	::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
}

double HRTimer::StopTimer(void)
{
	DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
	::QueryPerformanceCounter(&stop);
	::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
	return ((stop.QuadPart - start.QuadPart) / frequency);
}


bool CCustomStateHandler::OnInitialize()
{
	Deku2D::CAbstractScene *NewScene = Deku2D::SceneManager->CreateScene();
	Deku2D::SceneManager->SetCurrentScene(NewScene);
	Deku2D::LuaVirtualMachine
			->RunScript
			(
				Deku2D::Factory
				->Get<Deku2D::CScript>
				(
					Deku2D::Config->Section("Data")["InitScript"]
				)
			);

	HRTimer timer;
	timer.StartTimer();

	for
	(
		std::list<Deku2D::CResourceSectionLoaderBase*>::iterator i
	 = Deku2D::ResourceManager.Instance()->SectionsLoaders.begin();
	 i != Deku2D::ResourceManager.Instance()->SectionsLoaders.end();
	 ++i
	)
	{
		if((*i)->GetName() == "Fonts" )
			for
			(
				std::vector<std::pair<std::string, std::string> >::iterator j
					= (*i)->ResourceNames.begin();
				j != (*i)->ResourceNames.end();
				++j
			)
			{
				//Log( "RESOURCE", "Name: %s, filename: %s.", (*j).first.c_str(), (*j).second.c_str());
				Deku2D::Serialization::ToJSON(Deku2D::Factory->Get<Deku2D::CObject>((*j).first), "CObject", (*j).second + ".json" );
			}
	}

	double time = timer.StopTimer();
	Log( "TIME", "%lf", time );
	Deku2D::SLog->SetLogMode(Deku2D::CLog::LOG_MODE_STDOUT);
	Log( "TIME", "%lf", time );
	Deku2D::SLog->SetLogMode(Deku2D::CLog::LOG_MODE_STDERR);
	Log( "TIME", "%lf", time );

//	Deku2D::CEngine::Instance()
//			->RootGameObject
//			->Attach(Deku2D::Factory
//					 ->New<CTest>("SetSizeTest"));
//	//SoundMixer->PlayMusic(MusicManager->GetMusicByName("Iggy"), 0, -1);
//	Deku2D::SoundMixer->SetMusicVolume(128);

//	Deku2D::KeyBindingManager->UnbindAction("Test", "Act");
//	Deku2D::KeyBindingManager->Bind("Test", "Act", SDLK_0);
//	Deku2D::KeyBindingManager->Bind("Test", "Act", SDLK_RETURN);
//	Deku2D::KeyBindingManager->Bind("Test", "Act", SDLK_AMPERSAND);

//	if (!Deku2D::KeyBindingManager->IsBound(SDLK_m, "General", "Shoot"))
//	{
//		Deku2D::KeyBindingManager->Bind("General", "Shoot", SDLK_m);
//	}
//	Deku2D::KeyBindingManager->Save();

	//Deku2D::CResourceManager::Instance();

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
