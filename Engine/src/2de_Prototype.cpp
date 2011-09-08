#include "2de_Prototype.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CPrototype

	CPrototype::CPrototype()
	{
		Persistent = true; // persistent by default, at least now..
	}

	CPrototype::~CPrototype()
	{
		Unload();
	}

	bool CPrototype::Load()
	{
		if (Loaded)
			return true;

		if (Source == LOAD_SOURCE_FILE)
		{
			XML.LoadFromFile(Filename);
		}
		else if (Source == LOAD_SOURCE_MEMORY)
		{
			XML.LoadFromMemory(MemoryLoadData, MemoryLoadLength);
		}
		else
		{
			Log("ERROR", "Can't load prototype: no load source specified");
			return false;
		}

		if (XML.Root.First("Prototype")->IsErroneous() || !XML.Root.First("Prototype")->HasAttribute("Name"))
		{
			Log("ERROR", "Prototype named '%s' has invalid format", Name.c_str());
			return false;
		}

		CResource::BasicLoad();

		return true;
	}

	void CPrototype::Unload()
	{
		if (!Loaded)
			return;

		CResource::BasicUnload();
	}

	CXMLNode* CPrototype::GetRootNode()
	{
		CheckLoad();
		return XML.Root.First("Prototype");
	}

}	// namespace Deku2D