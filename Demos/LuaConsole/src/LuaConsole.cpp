#include "LuaConsole.h"

namespace LuaAPI
{

	// void ConsoleWrite(string Text)
	int ConsoleWrite(lua_State *L)
	{
		if (!lua_isstring(L, -1))
			CLuaVirtualMachine::Instance()->TriggerError("incorrect argument given to ConsoleWrite API call");

		CFactory::Instance()->Get<CLuaConsole>("Lua Console")->WriteLine(lua_tostring(L, -1));
		return 0;
	}

};

CLuaConsole::CLuaConsole()
{
	CEventManager::Instance()->Subscribe("KeyDown", this);

	CLuaVirtualMachine::Instance()->RegisterAPIFunction("ConsoleWrite", &LuaAPI::ConsoleWrite);

	MaxLines = (CGLWindow::Instance()->GetHeight() - 70) / 20; // ohlol.... need something like CFont::GetMaximumHeight()
	CurLines = 0;

	CommandHistory.push_front("");
	CommandHistoryIterator = CommandHistory.begin();

	//CommandLineEdit = CFactory::Instance()->New<CEdit>("CommandLineEdit");
	//CommandLineEdit->SetBox(CBox(10, 30, CGLWindow::Instance()->GetWidth() - 20, 30));

	CommandLineEdit = dynamic_cast<CGameObject *>(CFactory::Instance()->CreateByName("TempEdit", "CommandLineEdit"));
	CUpdateManager::Instance()->RootGameObject->Attach(CommandLineEdit);

	CommandOutput = CFactory::Instance()->New<CText>("CommandOutput");

	CPlaceableComponent *CommandOutputPlace = CFactory::Instance()->New<CPlaceableComponent>("CommandOutputPlace");
	CommandOutputPlace->SetPosition(Vector2(10, CGLWindow::Instance()->GetHeight() - 20));

	CommandOutputPlace->Attach(CommandOutput);
	CUpdateManager::Instance()->RootGameObject->Attach(CommandOutputPlace);

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
			// Это пиздец
			Input(dynamic_cast<CText *>(CommandLineEdit->Children[0]->Children[0]->Children[0]->Children[0])->GetText());
			break;
		case SDLK_UP:
			if (CommandHistoryIterator != --CommandHistory.end())
			{
				++CommandHistoryIterator;
				// Это пиздец
				dynamic_cast<CText *>(CommandLineEdit->Children[0]->Children[0]->Children[0]->Children[0])->SetText(*CommandHistoryIterator);
			}
			break;
		case SDLK_DOWN:
			if (CommandHistoryIterator != CommandHistory.begin())
			{
				--CommandHistoryIterator;
				// Это пиздец
				dynamic_cast<CText *>(CommandLineEdit->Children[0]->Children[0]->Children[0]->Children[0])->SetText(*CommandHistoryIterator);
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
	if (!CLuaVirtualMachine::Instance()->RunString(AText))
	{
		WriteLine(CLuaVirtualMachine::Instance()->GetLastError());
	}
	WritePrompt();
	dynamic_cast<CText *>(CommandLineEdit->Children[0]->Children[0]->Children[0]->Children[0])->SetText("");
}
