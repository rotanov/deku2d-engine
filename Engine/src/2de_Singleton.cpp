#include "2de_Singleton.h"

namespace Deku2D
{
	//////////////////////////////////////////////////////////////////////////
	// CSingletonManager

	CSingletonManager *CSingletonManager::_instance = NULL;

	CSingletonManager* CSingletonManager::Instance()
	{
		return _instance;
	}

	void CSingletonManager::Init()
	{
		_instance = new CSingletonManager;
	}

	void CSingletonManager::Add(CObject *AObject)
	{
		Singletones.push(AObject);
		//Log("NOTE", "ADDED TO SINGLETONE KILLER: %s", AObject->GetName().c_str());
	}

	void CSingletonManager::Clear()
	{
		while (!Singletones.empty())
		{
			delete Singletones.top();
			Singletones.pop();
		}
	}

	void CSingletonManager::Finalize()
	{
		delete _instance;
		_instance = NULL;
	}

}; // namespace Deku2D
