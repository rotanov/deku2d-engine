#include "2de_Event.h"

//////////////////////////////////////////////////////////////////////////
// CEventManager

CEventManager::~CEventManager()
{
	while (!Loop.empty())
	{
		delete Loop.front();
		Loop.pop();
	}
}

void CEventManager::Subscribe(const string &AEventName, CObject *Subscriber)
{
	Subscribers.insert(pair<string, CObject *>(AEventName, Subscriber));
}

void CEventManager::Unsubscribe(const string &AEventName, CObject *Subscriber)
{
	for (SubscribersContainer::iterator it = Subscribers.lower_bound(AEventName); it != Subscribers.upper_bound(AEventName); ++it)
	{
		if (it->second == Subscriber)
		{
			Subscribers.erase(it);
			return;
		}
	}
}

void CEventManager::TriggerEvent(const string &AEventName, CObject *ASender)
{
	Loop.push(new CEvent(AEventName, ASender));
}

void CEventManager::TriggerEvent(CEvent *AEvent)
{
	Loop.push(AEvent);
}

bool CEventManager::ProcessEvents()
{
	CEvent *event = NULL;

	while (!Loop.empty())
	{
		event = Loop.front();
		Loop.pop();

		for (SubscribersContainer::iterator it = Subscribers.lower_bound(event->GetName()); it != Subscribers.upper_bound(event->GetName()); ++it)
		{
			it->second->ProcessEvent(*event); 
		}

		delete event;
	}

	return true;
}
