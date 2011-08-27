#include "2de_Engine.h"

#define ENGINE Deku2d::CEngine::Instance()
#define RENDER Deku2d::CRenderManager::Instance()
Deku2d::CEngine* Ninja = ENGINE;

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
