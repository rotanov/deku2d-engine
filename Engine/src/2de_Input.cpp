#include "2de_Input.h"

#include "2de_Log.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CKeyBindingManager

	CKeyBindingManager::CKeyBindingManager() : ROOT_NODE_NAME("KeyBinding"), Filename(Environment::Paths::GetConfigPath() + ROOT_NODE_NAME + ".xml")
	{
		Load();
	}

	bool CKeyBindingManager::IsBound(SDLKey AKey, const string &ABindGroup, const string &ABindAction)
	{
		BindGroupsIterator git = Bindings.find(ABindGroup);
		if (git == Bindings.end())
			return false;

		for (BindsIterator bit = git->second.lower_bound(ABindAction); bit != git->second.upper_bound(ABindAction); ++bit)
		{
			if (bit->second == AKey)
				return true;
		}

		return false;
	}

	void CKeyBindingManager::Bind(const string &ABindGroup, const string &ABindAction, SDLKey AKey)
	{
		Bindings[ABindGroup].insert(make_pair(ABindAction, AKey));
	}

	void CKeyBindingManager::Unbind(const string &ABindGroup, const string &ABindAction, SDLKey AKey)
	{
		BindsContainer &bc = Bindings[ABindGroup];

		for (BindsIterator it = bc.lower_bound(ABindAction); it != bc.upper_bound(ABindAction); ++it)
		{
			if (it->second == AKey)
				bc.erase(it);
		}
	}

	void CKeyBindingManager::UnbindAction(const string &ABindGroup, const string &ABindAction)
	{
		BindsContainer &bc = Bindings[ABindGroup];
		bc.erase(ABindAction);

		if (bc.empty())
			Bindings.erase(ABindGroup);
	}

	void CKeyBindingManager::Load()
	{
		Bindings.clear();

		Data.LoadFromFile(Filename);

		CXMLNode *root = Data.Root.First(ROOT_NODE_NAME);
		if (root->IsErroneous())
		{
			Log("ERROR", "Key binding file doesn't exist or is in invalid format");
			return;
		}

		CXMLNode *group = NULL;
		CXMLNode *action = NULL;
		CXMLNode *key = NULL;

		for (CXMLNode::ChildrenIterator git = root->Children.Begin(); git != root->Children.End(); ++git)
		{
			group = *git;

			BindGroupsIterator gmit = Bindings.insert(make_pair(group->GetName(), BindsContainer())).first;

			for (CXMLNode::ChildrenIterator ait = group->Children.Begin(); ait != group->Children.End(); ++ait)
			{
				action = *ait;

				for (CXMLNode::ChildrenIterator kit = action->Children.Begin(); kit != action->Children.End(); ++kit)
				{
					key = *kit;

					if (key->GetName() != "Key" || !key->HasAttribute("ident"))
					{
						Log("WARNING", "Invalid key-binding config file format");
						continue;
					}

					gmit->second.insert(make_pair(action->GetName(), (SDLKey) from_string<unsigned int>(key->GetAttribute("ident"))));
				}
			}
		}

		Log("INFO", "Key bindings from '%s' loaded successfully", Filename.c_str());
	}

	void CKeyBindingManager::Save()
	{
		Data.Root.DeleteAll();

		Data.Root.AddLast(new CXMLPrologNode);
		CXMLNode *root = new CXMLNormalNode(ROOT_NODE_NAME);
		Data.Root.AddLast(root);

		CXMLNode *groupNode = NULL;
		CXMLNode *actionNode = NULL;
		CXMLNode *keyNode = NULL;
		string lastAction;

		for (BindGroupsIterator git = Bindings.begin(); git != Bindings.end(); ++git)
		{
			groupNode = new CXMLNormalNode(git->first);

			for (BindsIterator ait = git->second.begin(); ait != git->second.end(); ++ait)
			{
				if (ait->first != lastAction)
				{
					actionNode = new CXMLNormalNode(ait->first);
					groupNode->Children.AddLast(actionNode);
				}

				keyNode = new CXMLNormalNode("Key");
				keyNode->SetAttribute("ident", to_string<unsigned int>(ait->second));

				actionNode->Children.AddLast(keyNode);

				lastAction = ait->first;
			}
			root->Children.AddLast(groupNode);
		}

		Data.SaveToFile(Filename);
	}

}	//	namespace Deku2D
