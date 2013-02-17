#include "LuaConsole.h"
using namespace Deku2D;

namespace Deku2D
{
	namespace LuaAPI
	{

		// void ConsoleWrite(string Text)
		int ConsoleWrite(lua_State *L)
		{
			if (!lua_isstring(L, -1))
				LuaVirtualMachine->TriggerError("incorrect argument given to ConsoleWrite API call");

			Factory->Get<CLuaConsole>("Lua Console")->WriteLine(lua_tostring(L, -1));
			return 0;
		}

	}	//	namespace LuaAPI

}	//	namespace Deku2D

CLuaConsole::CLuaConsole()
{
	EventManager->Subscribe("KeyDown", this);

	LuaVirtualMachine->RegisterAPIFunction("ConsoleWrite", &LuaAPI::ConsoleWrite);

	MaxLines = (GLWindow->GetHeight() - 70) / 20; // ohlol.... need something like CFont::GetMaximumHeight()
	CurLines = 0;

	CommandHistory.push_front("");
	CommandHistoryIterator = CommandHistory.begin();

	//CommandLineEdit = Factory->New<CEdit>("CommandLineEdit");
	//CommandLineEdit->SetBox(CBox(10, 30, GLWindow->GetWidth() - 20, 30));

	CommandLineEdit = Factory->InstantiatePrototype("TempEdit", "CommandLineEdit");
	CEngine::Instance()->RootGameObject->Attach(CommandLineEdit);

	CommandOutput = Factory->New<CText>("CommandOutput");

	CPlaceableComponent *CommandOutputPlace = Factory->New<CPlaceableComponent>("CommandOutputPlace");
	CommandOutputPlace->SetPosition(Vector2(10, GLWindow->GetHeight() - 20));

	CommandOutputPlace->Attach(CommandOutput);
	CEngine::Instance()->RootGameObject->Attach(CommandOutputPlace);

	WriteLine("Welcome to Deku2D engine Lua console!");
	WritePrompt();
}

void CLuaConsole::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() == "KeyDown")
	{
		Uint16 key = AEvent.GetData<Uint16>("Sym");
		switch (key)
		{
		case SDLK_RETURN:
			// Это пиздец // а то!
			Input(dynamic_cast<CText *>(CommandLineEdit->GetChild(0)->GetChild(0)->GetChild(0)->GetChild(0))->GetText());
			break;
		case SDLK_UP:
			if (CommandHistoryIterator != --CommandHistory.end())
			{
				++CommandHistoryIterator;
				// Это пиздец
				dynamic_cast<CText *>(CommandLineEdit->GetChild(0)->GetChild(0)->GetChild(0)->GetChild(0))->SetText(*CommandHistoryIterator);
			}
			break;
		case SDLK_DOWN:
			if (CommandHistoryIterator != CommandHistory.begin())
			{
				--CommandHistoryIterator;
				// Это пиздец
				dynamic_cast<CText *>(CommandLineEdit->GetChild(0)->GetChild(0)->GetChild(0)->GetChild(0))->SetText(*CommandHistoryIterator);
			}
			break;
		}
	}
}

void CLuaConsole::Write(const string &AText)
{
	for (int i = 0; i < AText.length(); i++)
	{
		if (AText[i] == '\n')
			CleanUp();
	}
	CommandOutput->SetText(CommandOutput->GetText() + AText);
}

void CLuaConsole::WriteLine(const string &AText)
{
	Write(AText + "\n");
}

void CLuaConsole::CleanUp()
{
	CurLines++;
	if (CurLines > MaxLines)
	{
		CommandOutput->SetText(CommandOutput->GetText().substr(CommandOutput->GetText().find_first_of('\n') + 1));
		CurLines = MaxLines;
	}
}

void CLuaConsole::WritePrompt()
{
	Write("$ ");
}

void CLuaConsole::Input(const string &AText)
{
	CommandHistory.front() = AText;
	
	CommandHistory.push_front("");
	if (CommandHistory.size() > 200) // FIXME: magic number - size of history buffer
		CommandHistory.pop_back();

	CommandHistoryIterator = CommandHistory.begin();

	WriteLine(AText);
	if (!LuaVirtualMachine->RunString(AText))
	{
		WriteLine(LuaVirtualMachine->GetLastError());
	}
	WritePrompt();
	dynamic_cast<CText *>(CommandLineEdit->GetChild(0)->GetChild(0)->GetChild(0)->GetChild(0))->SetText("");
}
