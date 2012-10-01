#include "2de_Engine.h"
#include "2de_TypeInfo.h"

#define ENGINE Deku2D::CEngine::Instance()
#define RENDER Deku2D::CRenderManager::Instance()
Deku2D::CEngine* Ninja = ENGINE;

/*
Вот например у нас есть некий DraggeablePrototype;
Он должен обеспечивать придавание способности объектам быть перетаскиваемыми мышью.
[Root]->[Ball]->[Draggeabl] или [Root]->[Draggeable]->[Ball] 
Я хз что больше подходит для динамического присобачивания такого рода поведений для объектов, думаю первое всё таки.
В любом случае, Он хочет знать о хитбоксе объекта, к которому его присобачили. Либо Хочет знать, что событие клика по хитбоксу было и он хочет его перехватить/получить вместе с объектом. Потом он должен послать сообщение типа "Найди мне того, к кому меня присобачили, поидее, это должен быть ComponentRoot и передай первому своему PlaceableComponent'у что ему надо со мной поговорить (Или переместиться на столько-то)"

Так теперь надо чёткие пунктики отсюда вывести.
Этот самый Message passing как-то юниформно должен всплывать.
+ Сообщение вида "та срань, что в запросе в кавычках" прозрачно как-то придумать.
бля, ты мне там чё-то написал, я н в несостянии щас прочиатть, я пьян

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
			SerializeObjectToJSON((CObject*)Deku2D::CEngine::Instance()->RootGameObject, "rootdump.json");
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

bool CCustomStateHandler::OnInitialize()
{
	Deku2D::CAbstractScene *NewScene = Deku2D::SceneManager->CreateScene();
	Deku2D::SceneManager->SetCurrentScene(NewScene);
	Deku2D::LuaVirtualMachine->RunScript(Deku2D::Factory->Get<Deku2D::CScript>(Deku2D::Config->Section("Data")["InitScript"]));
	Deku2D::CEngine::Instance()->RootGameObject->Attach(Deku2D::Factory->New<CTest>("SetSizeTest"));
	//SoundMixer->PlayMusic(MusicManager->GetMusicByName("Iggy"), 0, -1);
	Deku2D::SoundMixer->SetMusicVolume(128);

	Deku2D::KeyBindingManager->UnbindAction("Test", "Act");
	Deku2D::KeyBindingManager->Bind("Test", "Act", SDLK_0);
	Deku2D::KeyBindingManager->Bind("Test", "Act", SDLK_RETURN);
	Deku2D::KeyBindingManager->Bind("Test", "Act", SDLK_AMPERSAND);

	if (!Deku2D::KeyBindingManager->IsBound(SDLK_m, "General", "Shoot"))
	{
		Deku2D::KeyBindingManager->Bind("General", "Shoot", SDLK_m);
	}
	Deku2D::KeyBindingManager->Save();
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
