#include "2de_Engine.h"

#define ENGINE CEngine::Instance()
#define RENDER CRenderManager::Instance()
CEngine* Ninja = ENGINE;

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

class CTest : public CGameObject
{
public:
	CTest()
	{
		CEventManager::Instance()->Subscribe("KeyDown", this);
	}

	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown" && AEvent.GetData<Uint16>("Sym") == SDLK_SPACE)
		{
			if (CGLWindow::Instance()->GetFullscreen())
			{
				CGLWindow::Instance()->SetFullscreen(false);
				CGLWindow::Instance()->SetSize(640, 480);
				CGLWindow::Instance()->Initialize();
			}
			else
			{
				/*CGLWindow::Instance()->SetSize(2048, 1152);
				CGLWindow::Instance()->SetFullscreen(true);*/
 				CGLWindow::Instance()->SetVideoMode(CGLWindow::Instance()->GetDesktopVideoMode());
			}
		}
	}

};

class CCustomStateHandler : public CAbstractStateHandler
{
public:
	bool OnInitialize();
	void OnFinalize();
};

bool CCustomStateHandler::OnInitialize()
{
	CAbstractScene *NewScene = CSceneManager::Instance()->CreateScene();
	CSceneManager::Instance()->SetCurrentScene(NewScene);
	CLuaVirtualMachine::Instance()->RunScript(CFactory::Instance()->Get<CScript>(CConfig::Instance()->Section("Data")["InitScript"]));
	CEngine::Instance()->RootGameObject->Attach(CFactory::Instance()->New<CTest>("SetSizeTest"));
	//CSoundMixer::Instance()->PlayMusic(CMusicManager::Instance()->GetMusicByName("Iggy"), 0, -1);
	CSoundMixer::Instance()->SetMusicVolume(128);
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
