#include <chrono>

#include "2de_Engine.h"
#include "2de_TypeInfo.h"
#include "2de_Resource.h"

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

using namespace std::chrono;
static high_resolution_clock::time_point timeStart;
static high_resolution_clock::time_point timeEnd;

void StartTimer()
{
	timeStart = high_resolution_clock::now();
}

void EndTimer(const char* message = "")
{
	timeEnd = high_resolution_clock::now();
	std::cout << message << " : " << duration_cast<microseconds>(timeEnd - timeStart).count() << "us" << std::endl;
}

class TestSerializationBase
{
	D2D_TYPE_INFO_INJECT(TestSerializationBase)

public:
	TestSerializationBase() : _x(0) {}
	~TestSerializationBase() {}
	int GetFoo() const { return _x; }
	void SetFoo(int foo) { _x = foo; }
	string GetStr() const { return _str; }
	void SetStr(const string& str) { _str = str; }

private:
	int _x;
	string _str;
};

D2D_TYPE_INFO_DECLARE(TestSerializationBase)
	D2D_DECLARE_PROPERTY_INFO(TestSerializationBase, int, Foo)
	D2D_DECLARE_PROPERTY_INFO(TestSerializationBase, string, Str)

class TestSerializationAbstractCase
{
public:
	virtual void Foo() = 0;
};

D2D_TYPE_INFO_DECLARE(TestSerializationAbstractCase)

void TestSerialization()
{
	using namespace Deku2D;

////////////////////////////////////////////////////////////////////////////////
	TypeInfo *charType = TypeInfo::GetTypeInfo("char");
	char *c = (char *) charType->New();
	*c = 'a';
	Log("REFLECTION", charType->GetString(c).c_str());
	Deku2D::CObject *obj = Deku2D::Factory->Get<Deku2D::CObject>("Iggy");
	TypeInfo *objType = TypeInfo::GetTypeInfo("CObject")->GetRunTimeTypeInfo(obj);
	Log("REFLECTION", objType->Name());
	Log("REFLECTION", ((string *)objType->FindProperty("Filename")->GetValue(obj))->c_str());

////////////////////////////////////////////////////////////////////////////////
	TestSerializationBase input;
	input.SetFoo(666);
	input.SetStr("Well, now I'm pretty sure I initialized it myself.");

	TestSerializationBase output;

	Deku2D::Serialization::ToJSON(&input, "TestSerializationBase", "input.json");
	Deku2D::Serialization::FromJSON(&output, "TestSerializationBase", "input.json");
	Deku2D::Serialization::ToJSON(&output, "TestSerializationBase", "output.json");

////////////////////////////////////////////////////////////////////////////////
	try
	{
		TypeInfo::GetTypeInfo("TestSerializationAbstractCase")->New();
	}
	catch(const std::runtime_error& e)
	{
		cout << e.what() << std::endl;
	}

////////////////////////////////////////////////////////////////////////////////
	FILE *f = fopen("typeinfo.txt", "w");
	fprintf(f, "%s", TypeInfo::GetTypeDescriptionString("CFont").c_str());
	fclose(f);

////////////////////////////////////////////////////////////////////////////////
	using namespace std::chrono;
	high_resolution_clock::time_point timeStart = high_resolution_clock::now();

	/*
	// I don't want to update compiler
	for (auto i : Deku2D::ResourceManager.Instance()->SectionsLoaders)
	{
		if (i->GetName() == "Fonts")
		{
			for (auto j : i->ResourceNames)
			{
				if (j.second.rfind(".fif", j.second.length() - 4) == j.second.length() - 4)
				{
					Log( "RESOURCE", "Name: %s, filename: %s.", j.first.c_str(), j.second.c_str());
					Deku2D::Serialization::ToJSON(Deku2D::Factory->Get<Deku2D::CObject>(j.first), "CObject", j.second + ".json" );
				}
			}
		}
	}
	*/

	for (auto i = Deku2D::ResourceManager->SectionsLoaders.begin(); i != Deku2D::ResourceManager->SectionsLoaders.end(); ++i)
	{
		if ((*i)->GetName() == "Fonts")
		{
			for (auto j = (*i)->ResourceNames.begin(); j != (*i)->ResourceNames.end(); ++j)
			{
				if (j->second.rfind(".fif", j->second.length() - 4) == j->second.length() - 4)
				{
					Log("RESOURCE", "Name: %s, filename: %s.", j->first.c_str(), j->second.c_str());
					Deku2D::Serialization::ToJSON(Deku2D::Factory->Get<Deku2D::CObject>(j->first), "CObject", j->second + ".json");
				}
			}

		}
	}

	high_resolution_clock::time_point timeEnd = high_resolution_clock::now();
	std::cout << duration_cast<microseconds>(timeEnd - timeStart).count() << "us" << std::endl;

////////////////////////////////////////////////////////////////////////////////
	try
	{
		CModel* model = Factory.Instance()->Get<CModel>("TestModel");
		cout << model->GetName() << std::endl;
		model->Load();

//		Deku2D::CAbstractScene *NewScene = Deku2D::SceneManager->CreateScene();
//		Deku2D::SceneManager->SetCurrentScene(NewScene);

		Serialization::ToJSON(model, "CModel", "TestModel.json");
		CModel newModel;
		//Serialization::FromJSON(&newModel, "CModel", "TestModel.json");
	}
	catch (const std::runtime_error& e)
	{
		cout << e.what() << std::endl;
	}
	catch (const char* e)
	{
		cout << e << std::endl;
	}
	catch (...)
	{
		cout << "Unkonwn exception." << std::endl;
	}
}

class CTest : public Deku2D::CGameObject
{
public:
	CTest()
	{
		Deku2D::EventManager->Subscribe("KeyDown", this);
	}

	void ProcessEvent(const Deku2D::CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown")
		{
			Uint16 sym = AEvent.GetData<Uint16>("Sym");
			if (sym == SDLK_SPACE)
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
					StartTimer();
					Deku2D::GLWindow->SetVideoMode(Deku2D::GLWindow->GetDesktopVideoMode());
					EndTimer("Progression to desktop video mode took");
				}
			}
			else if (sym == SDLK_e)
			{
				Deku2D::CGameObject *obj = Deku2D::Factory->Get<Deku2D::CGameObject>("Mouse cursor");
				obj->SetEnabled(!obj->IsEnabled());
			}
			else if (sym == SDLK_v)
			{
				Deku2D::CRenderableComponent *obj = Deku2D::Factory->Get<Deku2D::CRenderableComponent>("FPSText");
				obj->SetVisibility(!obj->GetVisibility());

			}
			else if (sym == SDLK_d)
			{
				//Deku2D::SerializeObjectToXml(Deku2D::CEngine::Instance()->RootGameObject, "rootdump.xml");
				//Deku2D::SerializeObjectToJSON(Deku2D::CEngine::Instance()->RootGameObject, "rootdump.json");
				Deku2D::Serialization::ToJSON(Deku2D::CEngine::Instance()->RootGameObject, "CObject", "rootdump.json");
				//Deku2D::DeserializeObjectFromJSON("rootdump.json");
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
	Deku2D::LuaVirtualMachine->RunScript
	(
		Deku2D::Factory->Get<Deku2D::CScript>
		(
			Deku2D::Config->Section("Data")["InitScript"]
		)
	);

	TestSerialization();

	Deku2D::CEngine::Instance()
			->RootGameObject
			->Attach(Deku2D::Factory
					 ->New<CTest>("SetSizeTest"));

	Deku2D::SoundMixer->PlayMusic(Deku2D::MusicManager->GetMusicByName("Iggy"), 0, -1);
	//Deku2D::SoundMixer->SetMusicVolume(128);

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
